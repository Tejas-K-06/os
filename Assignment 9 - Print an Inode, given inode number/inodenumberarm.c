#include <stdio.h>
//#include <linux/fs.h> //irrelvant to apple macbook 
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/types.h>
#include <stdint.h>
#include <ext2fs/ext2_fs.h>
#include <errno.h>

#define MAX_NUMBER_BLOCK_GROUPS 512

int main(int argc, char *argv[]) {
	struct ext2_super_block sb;
	int count = 0;
    if(argc != 3) {
        fprintf(stderr, "Usage: %s <filesystem_image> <inode_number>\n", argv[0]);
        exit(1);
    } //this is necessary to check whether the user have provided correct no. of args 
	int fd = open(argv[1], O_RDONLY);
	int inode_number = atoi(argv[2]);   //string to int conversion is needed 
	if(fd == -1) {
		perror("readsuper : ");
		exit(errno);
	}
	
	lseek(fd, 1024, SEEK_SET); //superblock is present at 1024 bytes 
	count = read(fd, &sb, sizeof(struct ext2_super_block)); //we count the number of bytes read from the superblock this will help us to check whether the superblock is read properly or not
    if(count != sizeof(struct ext2_super_block)) {
        fprintf(stderr, "Error reading superblock, size didnt match\n");
        exit(1);
    }
	
    //the important information needed from the superblock is:
    // total number of blocks
    // number of inodes
    // block size
    // blocks per group
    // inodes per group

	int block_size = ((1 << sb.s_log_block_size) * 1024);
    //ext2 defines blocksize = is in 1K bytes count so like 1K, 2K, 4K etc. so we need to multiply it by 1024 to get the actual block size in bytes

	struct ext2_group_desc grp_desc[MAX_NUMBER_BLOCK_GROUPS];
    //creates an array to hold block group descriptors 
    //block group descriptor basically tells us where the inode table is located for each block group and other information about the block group

	int number_of_inodes_per_group = sb.s_inodes_per_group;
    // Reads the number of inodes belonging to each block group from the superblock.

    //check the number of block groups and inode groups are same or not
    //i am trying to implement the thing stated in the ext2 documentation: https://wiki.osdev.org/Ext2 and extended info is given after this code snippet
    //took help of chatgpt in writing the given code snippet below
    uint64_t inode_groups =
        ((uint64_t)sb.s_inodes_count +
        sb.s_inodes_per_group - 1) /
        sb.s_inodes_per_group;

    uint64_t block_groups =
        ((uint64_t)sb.s_blocks_count +
        sb.s_blocks_per_group - 1) /
        sb.s_blocks_per_group;

    //very less chances of this happening; kind of unnecessary error checking code added by chatgpt but still added it to be on the safer side
    if (inode_groups != block_groups) {
        fprintf(stderr,
                "Inconsistent filesystem: inode groups = %llu, "
                "block groups = %llu\n",
                (unsigned long long)inode_groups,
                (unsigned long long)block_groups);

        close(fd);
        return EXIT_FAILURE;
    }

    // Check if the number of block groups exceeds the maximum allowed; i set it to 512
    if (inode_groups > MAX_NUMBER_BLOCK_GROUPS) {
        fprintf(stderr, "Too many block groups\n");

        close(fd);
        return EXIT_FAILURE;
    }

    int number_of_groups = (int)inode_groups;

    /*
    Determining the Number of Block Groups
    From the Superblock, extract the size of each block, the total number of inodes, the total number of blocks, the number of blocks per block group, and the number of inodes in each block group. From this information we can infer the number of block groups there are by:

    Rounding up the total number of blocks divided by the number of blocks per block group
    Rounding up the total number of inodes divided by the number of inodes per block group
    Both (and check them against each other)
    */


	int group_number_for_inode = (int)((inode_number - 1) / number_of_inodes_per_group);
    //calculates the block group containing the inode; note the -1 this is there because ext2 inode numbers start at 1

	int inode_index = (inode_number - 1) % number_of_inodes_per_group;
    //calculates the inodes index within the block group; again note the -1 for the same reason as above

    /*
    * For a 1 KiB block size, the group descriptor table
    * starts immediately after the superblock.
    */
    long long group_desc_location = 2048;

    lseek(fd, group_desc_location, SEEK_SET);
    // moves to byte offset 2048
    // for a 1 KB block-size ext2 filesystem, the group descriptor
    // table starts at byte offset 2048

    read(fd, &grp_desc,
         sizeof(struct ext2_group_desc) * number_of_groups);
    // reads all group descriptors into the grp_desc array

    struct ext2_group_desc req_desc =
        grp_desc[group_number_for_inode];
    // selects the descriptor for the block group containing the inode

    long long inode_table_location =
        (long long)req_desc.bg_inode_table * block_size;
    // bg_inode_table gives the first filesystem block occupied by
    // the inode table; multiply by block size to get byte offset

    long long inode_location =
        inode_table_location +
        (long long)inode_index * sb.s_inode_size;
    // calculates the byte location of the requested inode

    lseek(fd, inode_location, SEEK_SET);
    // moves to the beginning of the requested inode

    struct ext2_inode inode;

    read(fd, &inode, sizeof(struct ext2_inode));
    // reads the requested inode

    //Prints the requested inodes information
    //taken this from the ext2 documentation: https://wiki.osdev.org/Ext2 
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
	for (int i = 0; i < EXT2_N_BLOCKS; i++) {
		printf("i_block[%d]: %u\n", i, inode.i_block[i]);
	}
	printf("i_generation: %u\n", inode.i_generation);
	printf("i_file_acl: %u\n", inode.i_file_acl);
    printf("i_dir_acl: %u\n", inode.i_size_high);
	printf("i_faddr: %u\n", inode.i_faddr);
    //printf("osd1.linux1.l_i_reserved1: %u\n", inode.osd1.linux1.l_i_reserved1);
	// printf("osd2.linux2.l_i_frag: %u\n", inode.osd2.linux2.l_i_frag);
	// printf("osd2.linux2.l_i_fsize: %u\n", inode.osd2.linux2.l_i_fsize);
	// printf("osd2.linux2.i_pad1: %u\n", inode.osd2.linux2.i_pad1);
	// printf("osd2.linux2.l_i_uid_high: %u\n", inode.osd2.linux2.l_i_uid_high);
	// printf("osd2.linux2.l_i_gid_high: %u\n", inode.osd2.linux2.l_i_gid_high);
	// printf("osd2.linux2.l_i_reserved2: %u\n", inode.osd2.linux2.l_i_reserved2);

	return 0;
}