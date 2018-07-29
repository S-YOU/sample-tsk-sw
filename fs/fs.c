/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  File system                                                       */
/*                                                                    */
/**********************************************************************/

#include <kern/kernel.h>

inode_cache icache;

void
read_superblock(dev_id dev, superblock *sb){
    blk_buf *b;

    b = buffer_cache_blk_read(dev, SUPER_BLOCK_BLK_NO);
    memmove(sb, &b->data[0], sizeof(superblock));
    buffer_cache_blk_release(b);
}

static int 
find_free_data_block(dev_id dev, blk_buf **bp) {
	int64_t            i;
	superblock        sb;
	blk_buf           *b;
	blk_no           blk;
	blk_no    bitmap_blk;
	int64_t       offset;
	int64_t  chunk_index;
	int64_t chunk_offset;
	uint64_t         val;

	read_superblock(dev, &sb);
	for(i = 0; sb.s_nblocks > i; ++i ) {

		bitmap_blk = (RESV_BLOCK_NR + SUPER_BLOCK_BLK_NR + sb.s_imap_blocks) +
			(i / BITS_PER_BLOCK);

		b = buffer_cache_blk_read(dev, bitmap_blk);

		offset = i % BITS_PER_BLOCK;
		chunk_index = offset / sizeof(uint64_t);
		chunk_offset = offset % sizeof(uint64_t);

		val = *(( (uint64_t *)(&b->data[0]) ) + chunk_index);
		if ( !( val & ( 1ULL << chunk_offset ) ) ) {

			*bp = b;
			memset(&b->data[0], 0, BSIZE);
			buffer_cache_blk_release(b); /* TODO: Check correctness */
			return 0;
		}

		buffer_cache_blk_release(b);

	}

	return ENOENT;
}

static int 
release_data_block(dev_id dev, blk_no blk) {
	superblock        sb;
	blk_buf           *b;
	blk_no    bitmap_blk;
	int64_t       offset;
	int64_t  chunk_index;
	int64_t chunk_offset;
	uint64_t         val;

	read_superblock(dev, &sb);

	bitmap_blk = (RESV_BLOCK_NR + SUPER_BLOCK_BLK_NR + sb.s_imap_blocks) +
		(blk / BITS_PER_BLOCK);

	b = buffer_cache_blk_read(dev, bitmap_blk);

	offset = blk % BITS_PER_BLOCK;
	chunk_index = offset / sizeof(uint64_t);
	chunk_offset = offset % sizeof(uint64_t);
	val = *(( (uint64_t *)(&b->data[0]) ) + chunk_index);

	kassert( ( val & ( 1ULL << chunk_offset ) ) );

	*(( (uint64_t *)(&b->data[0]) ) + chunk_index) &= ~( 1ULL << chunk_offset );

	buffer_cache_blk_release(b);

	return 0;
}

/** Read disk i-node and returns busy buffer
 */
static int
read_disk_inode(dev_id dev, uint32_t ino, d_inode **dinodep, blk_buf  **bp) {
	int               rc;
	int64_t            i;
	superblock        sb;
	blk_no           blk;
	blk_buf           *b;
	int            index;

	read_superblock(dev, &sb);

	blk = (RESV_BLOCK_NR + SUPER_BLOCK_BLK_NR + sb.s_imap_blocks + sb.s_bmap_blocks) +
		(ino / INODES_PER_BLOCK);
	index = ino % INODES_PER_BLOCK;

	b = buffer_cache_blk_read(dev, blk);
	*dinodep = ( (d_inode *)(&b->data[0]) ) + index;
	*bp = b;

	return 0;
}

static int
write_disk_inode(dev_id dev, uint32_t ino, d_inode *dinode) {
	int        rc;
	int64_t     i;
	superblock sb;
	blk_no    blk;
	blk_buf    *b;
	int     index;

	read_superblock(dev, &sb);

	blk = (RESV_BLOCK_NR + SUPER_BLOCK_BLK_NR + sb.s_imap_blocks + sb.s_bmap_blocks) +
		(ino / INODES_PER_BLOCK);
	index = ino % INODES_PER_BLOCK;

	b = buffer_cache_blk_read(dev, blk);
	memmove( ( (d_inode *)(&b->data[0]) ) + index, dinode, sizeof(d_inode));

	buffer_cache_blk_write(b);
	buffer_cache_blk_release(b);

	return 0;
}

/** Find the inode by an i-node number
    and increase the i-node counter.
 */
static inode* 
inode_get (dev_id dev, uint32_t inum){
	int             i;
	inode *ip, *empty;

	mutex_hold(&icache.mtx);

	for (i = 0, empty = NULL; NINODE > i; ++i) {

		/* Search in the inode cache */
		ip = &icache.inode[i];
		if ( ( ip->ref > 0 ) && ( ip->i_dev == dev ) && ( ip->inum == inum ) ) {
			
			++ip->ref;  /* Found */
			goto out;
		}

		if ( ( empty == NULL ) && ( ip->ref == 0 ) ) 
			empty = ip;    /*  First empty slot.  */
	}
	
	kassert( empty != NULL );

	ip = empty;
	ip->i_dev = dev;
	ip->inum = inum;
	ip->ref = 1;
	ip->flags = 0;
out:
	mutex_release(&icache.mtx);
	return ip;
}

/** Sync i-node between a memory inode and a disk inode.
 */
void
inode_update(inode *ip){
	int        i;
	blk_buf  *bp;
	d_inode *dip;

	read_disk_inode(ip->i_dev, ip->inum, &dip, &bp);

	dip->i_mode = ip->i_mode;
	dip->i_dev = ip->i_dev;
	dip->i_nlink = ip->i_nlink;
	dip->i_size = ip->i_size;

	for(i = 0; FS_IADDR_NR > i; ++i) 
		dip->i_addr[i] = ip->i_addr[i];

	buffer_cache_blk_write(bp);
	buffer_cache_blk_release(bp);
}

/** Decrement inode reference by an i-node number
 */
void
inode_put(inode *ip){

	mutex_hold(&icache.mtx);
	if ( ( ip->ref == 1 ) && ( ip->flags & I_VALID ) && ( ip->i_nlink == 0 ) ){

		kassert( !(ip->flags & I_BUSY) );

		/*
		 * i-node has no links.
		 * Free file data blocks and then free inode.
		 */
		ip->flags |= I_BUSY;
		mutex_release(&icache.mtx);

		itrunc(ip);  /* Free data blocks */
		inode_update(ip);

		mutex_hold(&icache.mtx);
		ip->flags = 0;
		wque_wakeup(&ip->waiters, WQUE_REASON_WAKEUP);
	}

	--ip->ref;
	mutex_release(&icache.mtx);
}
void
inode_cache_init(void) {
	int i;

	mutex_init(&icache.mtx);
	for(i = 0; NINODE > i; ++i) {

		memset(&icache.inode[i], 0, sizeof(inode));
		wque_init_wait_queue(&(icache.inode[i].waiters));
	}
}