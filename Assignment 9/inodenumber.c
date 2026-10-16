#include<stdio.h>
#include<linux/fs.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<sys/types.h>
#include "ext2_fs.h"
#include<errno.h>

#define MAX_NUMBER_BLOCK_GROUPS 512

	
int main(int argc, char *argv[]) {
	struct ext2_super_block sb;
	int count = 0;
	int fd = open(argv[1], O_RDONLY);
	int inode_number = atoi(argv[2]);
	if(fd == -1){
		perror("readsuper : ");
		exit(errno);
	}
	
	lseek(fd, 1024, SEEK_SET); 
	count = read(fd, &sb, sizeof(struct ext2_super_block));
	
	//printf("The size of superblock is %ld\n",sizeof(struct ext2_super_block));
	int block_size = ((1 << sb.s_log_block_size) * 1024);
	//printf("The block size is %d\n",block_size);

	struct ext2_group_desc grp_desc[MAX_NUMBER_BLOCK_GROUPS];


	int number_of_inodes_per_group = sb.s_inodes_per_group;
    int number_of_groups = (sb.s_blocks_count + sb.s_blocks_per_group - 1) / sb.s_blocks_per_group;
	int group_number_for_inode = (int)((inode_number - 1) / number_of_inodes_per_group);
	int inode_index = ((inode_number - 1) % number_of_inodes_per_group);
    lseek(fd, block_size, SEEK_SET);
	read(fd, &grp_desc, sizeof(struct ext2_group_desc) * number_of_groups);       
	struct ext2_group_desc req_desc = grp_desc[group_number_for_inode];
	long long inode_table_location = (long long)req_desc.bg_inode_table * block_size;
    long long inode_location = inode_table_location + (long long)inode_index * sb.s_inode_size;
    lseek64(fd, inode_location, SEEK_SET);
    struct ext2_inode inode;
    read(fd, &inode, sizeof(struct ext2_inode));
		
	//printf("The bginode bitmap is at %d\n",req_desc.bg_inode_bitmap);
	//printf("The bg block bitmap is at %d\n",req_desc.bg_block_bitmap);
	//printf("The free indoes count is at %d\n",req_desc.bg_free_inodes_count);
	//printf("The free block count is at %d\n",req_desc.bg_free_blocks_count);
	
	printf("i_mode: %u\n", inode.i_mode);
	printf("i_uid: %u\n", inode.i_uid);
	printf("i_size: %u\n", inode.i_size);
	printf("i_atime: %u\n", inode.i_atime);
	printf("i_ctime: %u\n", inode.i_ctime);
	printf("i_mtime: %u\n", inode.i_mtime);
	printf("i_dtime: %u\n", inode.i_dtime);
	printf("i_gid: %u\n", inode.i_gid);
	printf("i_links_count: %u\n", inode.i_links_count);
	printf("i_blocks: %u\n", inode.i_blocks);
	printf("i_flags: %u\n", inode.i_flags);
	//printf("osd1.linux1.l_i_reserved1: %u\n", inode.osd1.linux1.l_i_reserved1);
	for (int i = 0; i < EXT2_N_BLOCKS; i++) {
		printf("i_block[%d]: %u\n", i, inode.i_block[i]);
	}
	printf("i_generation: %u\n", inode.i_generation);
	printf("i_file_acl: %u\n", inode.i_file_acl);
	printf("i_dir_acl: %u\n", inode.i_dir_acl);
	printf("i_faddr: %u\n", inode.i_faddr);
	//printf("osd2.linux2.l_i_frag: %u\n", inode.osd2.linux2.l_i_frag);
	// printf("osd2.linux2.l_i_fsize: %u\n", inode.osd2.linux2.l_i_fsize);
	// printf("osd2.linux2.i_pad1: %u\n", inode.osd2.linux2.i_pad1);
	// printf("osd2.linux2.l_i_uid_high: %u\n", inode.osd2.linux2.l_i_uid_high);
	// printf("osd2.linux2.l_i_gid_high: %u\n", inode.osd2.linux2.l_i_gid_high);
	// printf("osd2.linux2.l_i_reserved2: %u\n", inode.osd2.linux2.l_i_reserved2);
    close(fd);
	return 0;
}