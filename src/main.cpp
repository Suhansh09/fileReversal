#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 65536

/* =========================================================
   ============  COMMON / SHARED FUNCTIONS  ================
   (reversebuffer was identical in both Q1 and Q2, kept once)
   ========================================================= */

// reversing the buffer as per later flags requirements
void reversebuffer(char *buf, int len){
    int i = 0;
    int j = len - 1;

    while (i < j)
    {
        char temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;

        i++;
        j--;
    }
}


/* =========================================================
   ================  Q1 FUNCTIONS (PROCESS)  ================
   ========================================================= */

//progress function to show the progress bar of the completed work
void progress(long long written, long long total){

    if (total == 0){
        printf("\rProgress: 100.00%%");
        fflush(stdout);
        return;
    }
    double percent = ((double)written / total) * 100.0;

    printf("\rProgress: %.2f%%", percent);
    fflush(stdout);
}

//function to write complete bytes 
// sometime write does partial write to make sure it does not do it 
int writecomplete(int fd, char* buffer, long long bytes){
    
    long long written = 0;
    while (written < bytes){

    ssize_t n = write(fd, buffer + written, bytes - written);

    if (n == -1){
        perror("Error writing file");
        return -1;
      }

    written += n;
  }

  return 0;
}  



// if the file is given by complete pathname
const char* basename(const char *path){
    const char *base = strrchr(path, '/');
    if (base)
        return base + 1;
    return path;
}

//creating output directory
int outputdirectory(){
    struct stat st;

    if (stat("Assignment1", &st) == -1)
    {
        int a = mkdir("Assignment1", 0700);

        if (a == -1)
        {
            perror("Error creating directory Assignment1");
            return -1;
        }
    }

    return 0;
}


//flag 0 reversal
int flag0rev(const char *inputfile, int blocksize){

    int infd = open(inputfile,O_RDONLY);

     if(infd==-1){
        perror("Error opening input file");
            return -1;
        }

    //get file size using lseek
    long long filesize = lseek(infd,0,SEEK_END);
    lseek(infd,0,SEEK_SET);

    const char*basen = basename(inputfile);

    //creating output file
    char outpath[1024];

    // creates output file in the outpath string
    snprintf(outpath,sizeof(outpath),"Assignment1/0_%s",basen);

    int outfd = open(outpath,O_CREAT|O_WRONLY|O_TRUNC,0600);

    if(outfd==-1){
        perror("Error creating output file");
        close(infd);
        return -1;
    }
    
      char *buffer = (char *)malloc(BUFFER_SIZE);
     if (!buffer) {
        perror("Memory allocation failed");
        close(infd);
        close(outfd);
        return -1;
    }

    long long totalwritten=0;
    long long blockstart = 0;
 
    while (blockstart < filesize){
        
        long long blocklen = blocksize;
        if (blockstart + blocklen > filesize)
            blocklen = filesize - blockstart;
        
        // this makes pos at the last position of the block 
        // remaining ensures that even though if the blocksize is bigger then the buffer then by
        // breaking it into smaller chunks there is no
        long long pos = blockstart + blocklen; 
        long long remaininginblock = blocklen;
 
        while (remaininginblock > 0){
            long long chunk;
            if( remaininginblock < BUFFER_SIZE)chunk=remaininginblock;
            else chunk=  BUFFER_SIZE;
            pos -= chunk;
 
            if (lseek(infd, pos, SEEK_SET) == -1){
                perror("Error seeking input file");
                free(buffer);
                close(infd);
                close(outfd);
                return -1;
            }
 
            long long bytesread = read(infd, buffer, chunk);
            if (bytesread <= 0){
                perror("Error reading input file");
                free(buffer);
                close(infd);
                close(outfd);
                return -1;
            }
 
            reversebuffer(buffer, bytesread);
 
            if (writecomplete(outfd, buffer, bytesread) == -1){
                free(buffer);
                close(infd);
                close(outfd);
                return -1;
            }
 
            totalwritten += bytesread;
            remaininginblock -= bytesread;
            progress(totalwritten, filesize);
        }
 
        blockstart += blocklen;
    }
   
  printf("\rProgress: 100.00%%\n");
  printf("Block-wise reversal complete. Output: %s\n", outpath);

  free(buffer);
  close(infd);
  close(outfd);

  return 0;

}

// flag 1 reversal
int flag1rev(const char *inputfile){
     int infd = open(inputfile,O_RDONLY);

     if(infd==-1){
        perror("Error opening input file");
            return -1;
        }

    
    long long filesize = lseek(infd,0,SEEK_END);
    if(filesize == -1){
    perror("Error getting file size");
    close(infd);
    return -1;
    }
    

    const char*basen = basename(inputfile);

   
    char outpath[1024];
    snprintf(outpath,sizeof(outpath),"Assignment1/1_%s",basen);
    int outfd = open(outpath,O_CREAT|O_WRONLY|O_TRUNC,0600);

    if(outfd==-1){
        perror("Error creating output file");
        close(infd);
        return -1;
    }
    
      char *buffer = (char *)malloc(BUFFER_SIZE);
     if (!buffer) {
        perror("Memory allocation failed");
        close(infd);
        close(outfd);
        return -1;
    }

    long long totalwritten=0;
    long long remaining = filesize;
    long long bytesread;

  while(remaining > 0){
    long long chunksz;

    if(remaining < BUFFER_SIZE)
        chunksz = remaining;
    else
        chunksz = BUFFER_SIZE;

    lseek(infd, -chunksz, SEEK_CUR);

    bytesread = read(infd, buffer, chunksz);

    if(bytesread == -1){
        perror("Error reading input file");
        free(buffer);
        close(infd);
        close(outfd);
        return -1;
    }

    reversebuffer(buffer, bytesread);

    if(writecomplete(outfd, buffer, bytesread) == -1){
        free(buffer);
        close(infd);
        close(outfd);
        return -1;
    }

    totalwritten += bytesread;
    remaining -= bytesread;

    progress(totalwritten, filesize);

    lseek(infd, -bytesread, SEEK_CUR);
   } 

    printf("\rProgress: 100.00%%\n");
    printf("Full file reversal complete. Output: %s\n", outpath);

    free(buffer);
    close(infd);
    close(outfd);

    return 0;


}

//flag 2 reversal
int flag2rev(const char *inputfile, long long start, long long end)
{
    int infd = open(inputfile, O_RDONLY);

    if(infd == -1){
        perror("Error opening input file");
        return -1;
    }

    long long filesize = lseek(infd, 0, SEEK_END);

    if(filesize == -1){
        perror("Error getting file size");
        close(infd);
        return -1;
    }

    if(start < 0 || end < start || end >= filesize){
        fprintf(stderr, "Invalid start and end indices\n");
        close(infd);
        return -1;
    }

    const char *basen = basename(inputfile);

    char outpath[1024];

    snprintf(outpath, sizeof(outpath),"Assignment1/2_%s", basen);

    int outfd = open(outpath, O_CREAT | O_WRONLY | O_TRUNC, 0600);

    if(outfd == -1){
        perror("Error creating output file");
        close(infd);
        return -1;
    }

    char *buffer = (char *)malloc(BUFFER_SIZE);

    if(buffer == NULL){
        perror("Memory allocation failed");
        close(infd);
        close(outfd);
        return -1;
    }

    long long totalwritten = 0;
    long long bytesread;

    //  reverse bytes before start
    if(start > 0){
        long long remaining = start;

        while(remaining > 0){
            long long chunksz;

            if(remaining < BUFFER_SIZE)
                chunksz = remaining;
            else
                chunksz = BUFFER_SIZE;
            
            // done to make offset at the buffer from the end side(near the start) and does file reversing
            // like normal file but for 0 to start-1
            remaining -= chunksz;

            if(lseek(infd, remaining, SEEK_SET) == -1){
                perror("Error seeking input file");
                free(buffer);
                close(infd);
                close(outfd);
                return -1;
            }

            bytesread = read(infd, buffer, chunksz);

            if(bytesread <= 0){
                perror("Error reading input file");
                free(buffer);
                close(infd);
                close(outfd);
                return -1;
            }

            reversebuffer(buffer, bytesread);

            if(writecomplete(outfd, buffer, bytesread) == -1) {
                free(buffer);
                close(infd);
                close(outfd);
                return -1;
            }

            totalwritten += bytesread;
            progress(totalwritten, filesize);
        }
    }

    // copy middle part unchanged
    if(lseek(infd, start, SEEK_SET) == -1){
        perror("Error seeking input file");
        free(buffer);
        close(infd);
        close(outfd);
        return -1;
    }

    long long middlep = end - start + 1;

    while(middlep > 0){
        long long chunksz;

        if(middlep < BUFFER_SIZE)
            chunksz = middlep;
        else
            chunksz = BUFFER_SIZE;

        bytesread = read(infd, buffer, chunksz);

        if(bytesread <= 0){
            perror("Error reading input file");
            free(buffer);
            close(infd);
            close(outfd);
            return -1;
        }

        if(writecomplete(outfd, buffer, bytesread) == -1){
            free(buffer);
            close(infd);
            close(outfd);
            return -1;
        }

        middlep -= bytesread;
        totalwritten += bytesread;

        progress(totalwritten, filesize);
    }

    // reverse bytes after end
    if(end < filesize - 1){
        long long remaining = filesize - end - 1;

        while(remaining > 0){
            long long chunksz;

            if(remaining < BUFFER_SIZE)
                chunksz = remaining;
            else
                chunksz = BUFFER_SIZE;

            remaining -= chunksz;

            if(lseek(infd, end + 1 + remaining, SEEK_SET) == -1){
                perror("Error seeking input file");
                free(buffer);
                close(infd);
                close(outfd);
                return -1;
            }

            bytesread = read(infd, buffer, chunksz);

            if(bytesread <= 0){
                perror("Error reading input file");
                free(buffer);
                close(infd);
                close(outfd);
                return -1;
            }

            reversebuffer(buffer, bytesread);

            if(writecomplete(outfd, buffer, bytesread) == -1){
                free(buffer);
                close(infd);
                close(outfd);
                return -1;
            }

            totalwritten += bytesread;
            progress(totalwritten, filesize);
        }
    }

    printf("\rProgress: 100.00%%\n");
    printf("Partial range reversal complete. Output: %s\n", outpath);

    free(buffer);
    close(infd);
    close(outfd);

    return 0;
}


/* =========================================================
   ================  Q2 FUNCTIONS (VERIFY)  =================
   ========================================================= */

// printing permissions using struct and the file
void printpermissions(struct stat *st, const char *label){
    int r_usr = st ? (st->st_mode & S_IRUSR) : 0;
    int w_usr = st ? (st->st_mode & S_IWUSR) : 0;
    int x_usr = st ? (st->st_mode & S_IXUSR) : 0;
    
    int r_grp = st ? (st->st_mode & S_IRGRP) : 0;
    int w_grp = st ? (st->st_mode & S_IWGRP) : 0;
    int x_grp = st ? (st->st_mode & S_IXGRP) : 0;
    
    int r_oth = st ? (st->st_mode & S_IROTH) : 0;
    int w_oth = st ? (st->st_mode & S_IWOTH) : 0;
    int x_oth = st ? (st->st_mode & S_IXOTH) : 0;

    printf("User has read permissions on %s: %s\n", label, r_usr ? "Yes" : "No");
    printf("User has write permission on %s: %s\n", label, w_usr ? "Yes" : "No");
    printf("User has execute permission on %s: %s\n", label, x_usr ? "Yes" : "No");
    printf("Group has read permissions on %s: %s\n", label, r_grp ? "Yes" : "No");
    printf("Group has write permission on %s: %s\n", label, w_grp ? "Yes" : "No");
    printf("Group has execute permission on %s: %s\n", label, x_grp ? "Yes" : "No");
    printf("Others has read permissions on %s: %s\n", label, r_oth ? "Yes" : "No");
    printf("Others has write permission on %s: %s\n", label, w_oth ? "Yes" : "No");
    printf("Others has execute permission on %s: %s\n", label, x_oth ? "Yes" : "No");
}

// verification of flag0
int verifyflag0(const char *newfile, const char *oldfile, int blocksize){

    int newfd = open(newfile, O_RDONLY);
    int oldfd = open(oldfile, O_RDONLY);

    if(newfd == -1 || oldfd == -1){
        if(newfd != -1)
            close(newfd);

        if(oldfd != -1)
            close(oldfd);

        return 0;
    }

    
    long long filesz = lseek(oldfd, 0, SEEK_END);

    if(filesz == -1){
        perror("Error getting file size");
        close(oldfd);
        close(newfd);
        return 0;
    }

    
    long long newfilesz = lseek(newfd, 0, SEEK_END);

    if(newfilesz == -1){
        perror("Error getting file size");
        close(oldfd);
        close(newfd);
        return 0;
    }

   
    if(filesz != newfilesz){
        close(oldfd);
        close(newfd);
        return 0;
    }

    char *oldbuf = (char *)malloc(BUFFER_SIZE);
    char *newbuf = (char *)malloc(BUFFER_SIZE);

    if(oldbuf == NULL || newbuf == NULL){
        perror("Memory allocation failed");

        free(oldbuf);
        free(newbuf);

        close(oldfd);
        close(newfd);

        return 0;
    }

    int correct = 1;
    long long blockstart = 0;

     while(blockstart < filesz){

        long long trueblocksz = blocksize;

        if(blockstart + trueblocksz > filesz)
            trueblocksz = filesz - blockstart;

        long long processed = 0;

        while(processed < trueblocksz){

            long long remaining = trueblocksz - processed;

            long long chunksz = BUFFER_SIZE;

            if(remaining < chunksz)
                chunksz = remaining;

             long long oldp = blockstart + processed;

            if(lseek(oldfd, oldp, SEEK_SET) == -1){
                perror("Error seeking old file");
                correct = 0;
                break;
            }

            ssize_t oldr = read(oldfd, oldbuf, chunksz);

            if(oldr != chunksz){
                correct = 0;
                break;
            }

            
            long long newp = blockstart + trueblocksz - processed - chunksz;

            if(lseek(newfd, newp, SEEK_SET) == -1){
                perror("Error seeking new file");
                correct = 0;
                break;
            }

            ssize_t newr = read(newfd, newbuf ,chunksz);

            if(newr != chunksz){
                correct = 0;
                break;
            }
     reversebuffer(oldbuf, oldr);

            for(long long i = 0; i < oldr; i++){

                if(oldbuf[i] != newbuf[i]){
                    correct = 0;
                    break;
                }
            }

            if(!correct)
                break;

            processed += oldr;
        }

        if(!correct)
            break;

        blockstart += trueblocksz;
    }

    free(oldbuf);
    free(newbuf);

    close(oldfd);
    close(newfd);

    return correct;
}


int verifyflag1(const char *newfile, const char *oldfile) {

    int newfd = open(newfile, O_RDONLY);
    int oldfd = open(oldfile, O_RDONLY);

    if(newfd == -1 || oldfd == -1) {

        if(newfd != -1)
            close(newfd);

        if(oldfd != -1)
            close(oldfd);

        return 0;
    }

   
    long long filesz = lseek(oldfd, 0, SEEK_END);

    if(filesz == -1){
        perror("Error getting file size");
        close(oldfd);
        close(newfd);
        return 0;
    }

    
    long long newfilesz = lseek(newfd, 0, SEEK_END);

    if(newfilesz == -1){
        perror("Error getting file size");
        close(oldfd);
        close(newfd);
        return 0;
    }

     if(filesz != newfilesz){
        close(oldfd);
        close(newfd);
        return 0;
    }

    char *oldbuf = (char *)malloc(BUFFER_SIZE);
    char *newbuf = (char *)malloc(BUFFER_SIZE);

    if(oldbuf == NULL || newbuf == NULL){
        perror("Memory allocation failed");

        free(oldbuf);
        free(newbuf);

        close(oldfd);
        close(newfd);

        return 0;
    }

    int correct = 1;

    long long newp = filesz;

    while(1){

        ssize_t oldr = read(oldfd, oldbuf, BUFFER_SIZE);

        if(oldr <= 0)
            break;

        newp -= oldr;
          if (lseek(newfd, newp, SEEK_SET) == -1){
            perror("Error seeking new file");
            correct = 0;
            break;
        }

        ssize_t newr = read(newfd, newbuf, oldr);

        if (newr != oldr) {
            correct = 0;
            break;
        }

        
        reversebuffer(newbuf, newr);

       
        for(ssize_t i = 0; i < oldr; i++){

            if (oldbuf[i] != newbuf[i]){
                correct = 0;
                break;
            }
        }

        if(!correct)
            break;
    }

    free(oldbuf);
    free(newbuf);

    close(oldfd);
    close(newfd);

    return correct;
}

int verifyreversedrange(int newfd, int oldfd, long long rangestart, long long rangeend) {

    long long rangesz = rangeend - rangestart + 1;

    if(rangesz <= 0)
        return 1;

    char *oldbuf = (char *)malloc(BUFFER_SIZE);
    char *newbuf = (char *)malloc(BUFFER_SIZE);

    if(oldbuf == NULL || newbuf == NULL){
        free(oldbuf);
        free(newbuf);
        return 0;
    }

    int correct = 1;

    long long oldpos = rangestart;
    long long newpos = rangeend + 1;

    while(oldpos <= rangeend){

        long long chunksz = BUFFER_SIZE;

        if(rangeend - oldpos + 1 < chunksz)
            chunksz = rangeend - oldpos + 1;

        if(lseek(oldfd, oldpos, SEEK_SET) == -1){
            correct = 0;
            break;
        }
 ssize_t oldr = read(oldfd, oldbuf, chunksz);

        if(oldr <= 0){
            correct = 0;
            break;
        }

        newpos -= oldr;

        if(lseek(newfd, newpos, SEEK_SET) == -1){
            correct = 0;
            break;
        }

        ssize_t newr = read(newfd, newbuf, oldr);

        if(newr != oldr) {
            correct = 0;
            break;
        }

        reversebuffer(newbuf, newr);

        for(ssize_t i = 0; i < oldr; i++){

            if (oldbuf[i] != newbuf[i]) {
                correct = 0;
                break;
            }
        }

        if(!correct)
            break;

        oldpos += oldr;
    }
     free(oldbuf);
    free(newbuf);

    return correct;
}

int verifyidenticalrange(int newfd, int oldfd, long long rangestart, long long rangeend){

    long long rangesz = rangeend - rangestart + 1;

    if(rangesz <= 0)
        return 1;

    char *oldbuf = (char *)malloc(BUFFER_SIZE);
    char *newbuf = (char *)malloc(BUFFER_SIZE);

    if(oldbuf == NULL || newbuf == NULL){
        free(oldbuf);
        free(newbuf);
        return 0;
    }

    int correct = 1;

    long long remaining = rangesz;

    if(lseek(oldfd, rangestart, SEEK_SET) == -1 || lseek(newfd, rangestart, SEEK_SET) == -1){

        free(oldbuf);
        free(newbuf);
        return 0;
    }

    while(remaining > 0){

        long long chunksz = BUFFER_SIZE;

        if(remaining < chunksz)
            chunksz = remaining;

        ssize_t oldr = read(oldfd, oldbuf, chunksz);
        ssize_t newr = read(newfd, newbuf, chunksz);

         if (oldr != newr || oldr <= 0){
            correct = 0;
            break;
        }

        for(ssize_t i = 0; i < oldr; i++){

            if(oldbuf[i] != newbuf[i]) {
                correct = 0;
                break;
            }
        }

        if(!correct)
            break;

        remaining -= oldr;
    }

    free(oldbuf);
    free(newbuf);

    return correct;
}

int verifyflag2(const char *newfile, const char *oldfile,long long startidx, long long endidx){

    int newfd = open(newfile, O_RDONLY);
    int oldfd = open(oldfile, O_RDONLY);

    if(newfd == -1 || oldfd == -1){

        if(newfd != -1)
            close(newfd);

        if(oldfd != -1)
            close(oldfd);

        return 0;
    }

    long long filesz = lseek(oldfd, 0, SEEK_END);

    if(filesz == -1){
        perror("Error getting file size");
        close(oldfd);
        close(newfd);
        return 0;
    }

    long long newfilesz = lseek(newfd, 0, SEEK_END);

    if(newfilesz == -1){
        perror("Error getting file size");
        close(oldfd);
        close(newfd);
        return 0;
    }
     if(filesz != newfilesz){
        close(oldfd);
        close(newfd);
        return 0;
    }

    if(startidx < 0 || endidx < startidx || endidx >= filesz){
        close(oldfd);
        close(newfd);
        return 0;
    }

    int correct = 1;

    if(startidx > 0) {

        if(!verifyreversedrange(newfd, oldfd, 0, startidx - 1)){
            correct = 0;
        }
    }

    if(correct) {

        if(!verifyidenticalrange(newfd, oldfd,startidx, endidx)){
            correct = 0;
        }
    }
 if(correct && endidx < filesz - 1) {

        if(!verifyreversedrange(newfd, oldfd,endidx + 1, filesz - 1)){
            correct = 0;
        }
    }

    close(oldfd);
    close(newfd);

    return correct;
}


/* =========================================================
   ==============  Q1 main (renamed helper)  ================
   ========================================================= */

int process_main(int argc, char *argv[]){
    // argv[0] here is "process", so shift by one: real args start at argv[1]
    if(argc < 3){
        printf("Usage:\n");
        printf("  ./a.out process <input file> 0 <block size>\n");
        printf("  ./a.out process <input file> 1\n");
        printf("  ./a.out process <input file> 2 <start index> <end index>\n");
        return 1;
    }

    const char *inputfile = argv[1];
    int flag = atoi(argv[2]);

    // Check whether input file exists
    struct stat st;

    if (stat(inputfile, &st) == -1){
        perror("Error: Input file does not exist");
        return 1;
    }

    // Create Assignment1 directory
    if (outputdirectory() == -1){
        return 1;
    }

    switch (flag){
        case 0:{
            if (argc < 4){
                printf("Error: Block size required for flag 0.\n");
                printf("Usage: ./a.out process <input file> 0 <block size>\n");
                return 1;
            }

            int blocksize = atoi(argv[3]);

            if (blocksize <= 0){
                printf("Error: Block size must be a positive integer.\n");
                return 1;
            }

            return flag0rev(inputfile, blocksize);
        }

        case 1:{
            return flag1rev(inputfile);
        }

        case 2:{
            if (argc < 5){
                printf("Error: Start and end indices required for flag 2.\n");
                printf("Usage: ./a.out process <input file> 2 <start index> <end index>\n");
                return 1;
            }

            long long start = atoll(argv[3]);
            long long end = atoll(argv[4]);

            return flag2rev(inputfile, start, end);
        }

        default:{
            printf("Error: Invalid flag. Use 0, 1, or 2.\n");
            return 1;
        }
    }
}


/* =========================================================
   ==============  Q2 main (renamed helper)  ================
   ========================================================= */

int verify_main(int argc, char *argv[]){
    // argv[0] here is "verify", so shift by one: real args start at argv[1]
    if(argc < 5) {
        printf("Usage:\n");
        printf("  Flag 0: ./a.out verify <newfile> <oldfile> <directory> 0 <blocksize>\n");
        printf("  Flag 1: ./a.out verify <newfile> <oldfile> <directory> 1\n");
        printf("  Flag 2: ./a.out verify <newfile> <oldfile> <directory> 2 <start> <end>\n");
        return 1;
    }

    const char *newfile = argv[1];
    const char *oldfile = argv[2];
    const char *dirpath = argv[3];

    int flag = atoi(argv[4]);

    struct stat newfilestat, oldfilestat, dirstat;

    int direxists = 1;
    int newfileexists = 1;
    int oldfileexists = 1;

    if(stat(dirpath, &dirstat) == -1){
        direxists = 0;
    }

    if(stat(newfile, &newfilestat) == -1){
        newfileexists = 0;
    }

    if(stat(oldfile, &oldfilestat) == -1){
        oldfileexists = 0;
    }

    
    printf("Directory is created: %s\n",direxists ? "Yes" : "No");
    int contentcorrect = 0;

    if(newfileexists && oldfileexists){

        switch(flag){

            case 0:{

                if(argc < 6){
                    printf("Error: Block size required for flag 0.\n");
                    printf("Usage: ./a.out verify <newfile> <oldfile> <directory> 0 <blocksize>\n");
                    return 1;
                }

                int blocksize = atoi(argv[5]);

                if(blocksize <= 0){
                    printf("Error: Block size must be a positive integer.\n");
                    return 1;
                }

                contentcorrect =verifyflag0(newfile, oldfile, blocksize);

                break;
            }

            case 1:{

                contentcorrect =
                    verifyflag1(newfile, oldfile);

                break;
            }

            case 2:{

                if (argc < 7){
                    printf("Error: Start and end indices required for flag 2.\n");
                    printf("Usage: ./a.out verify <newfile> <oldfile> <directory> 2 <start> <end>\n");
                    return 1;
                }

                long long startidx = atoll(argv[5]);
                long long endidx = atoll(argv[6]);

                contentcorrect =verifyflag2(newfile, oldfile,startidx, endidx);

                break;
            }

            default:{
                printf("Error: Invalid flag. Use 0, 1, or 2.\n");
                return 1;
            }
        }
    }

    printf("Whether file contents are correctly processed: %s\n",contentcorrect ? "Yes" : "No");

    
     int samesize = 0;

    if(newfileexists && oldfileexists) {
        samesize =(newfilestat.st_size == oldfilestat.st_size);
    }

    printf("Both Files Sizes are Same: %s\n",samesize ? "Yes" : "No");

   
    printpermissions(newfileexists ? &newfilestat : NULL, "newfile");
    printpermissions(oldfileexists ? &oldfilestat : NULL, "oldfile");
    printpermissions(direxists ? &dirstat : NULL, "directory");
    
    return 0;
}


/* =========================================================
   ======================  COMBINED main  ====================
   Dispatches to process_main() (old Q1::main) or
   verify_main() (old Q2::main) based on the first argument.
   ========================================================= */

int main(int argc, char *argv[]){

    if(argc < 2){
        printf("Usage:\n");
        printf("  ./a.out process <input file> <flag> [args]\n");
        printf("  ./a.out verify <newfile> <oldfile> <directory> <flag> [args]\n");
        return 1;
    }

    if(strcmp(argv[1], "process") == 0){
        return process_main(argc - 1, argv + 1);
    }
    else if(strcmp(argv[1], "verify") == 0){
        return verify_main(argc - 1, argv + 1);
    }
    else{
        printf("Error: Invalid mode. Use 'process' or 'verify'.\n");
        printf("Usage:\n");
        printf("  ./a.out process <input file> <flag> [args]\n");
        printf("  ./a.out verify <newfile> <oldfile> <directory> <flag> [args]\n");
        return 1;
    }
}
