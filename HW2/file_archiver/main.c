#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>

#define BUFFER_SIZE 32

// DEBUG
bool DEBUG = false;

void pack(char* archive_filename, char* src_directory);
void unpack(char* archive_filename, char* dest_directory);
void add_file(char* archive_filename, char* target_filename);
void del_file(char* archive_filename, char* target_filename);
void list(char* archive_filename);
int isBinaryFile(char* filename);

int main(int argc, char** argv){
    // argv를 받아서 해석하는 부분
    char* archive_filename;
    char* target_filename;
    char* directory;
    // 0 : pack, 1 : unpack, 2 : del, 3 : add, 4 : list
    unsigned int func_indicator = 0;

    for(int i=1;i<argc;i++){
        if(strcmp(argv[i], "pack") == 0){ // pack일때
            func_indicator = 0;
            i++;
            archive_filename = argv[i];
            i++;
            directory = argv[i];
        }
        else if(strcmp(argv[i], "unpack") == 0){
            func_indicator = 1;
            i++;
            archive_filename = argv[i];
            i++;
            directory = argv[i];
        }
        else if(strcmp(argv[i], "del") == 0){
            func_indicator = 2;
            i++;
            archive_filename = argv[i];
            i++;
            target_filename = argv[i];
        }
        else if(strcmp(argv[i], "add") == 0){
            func_indicator = 3;
            i++;
            archive_filename = argv[i];
            i++;
            target_filename = argv[i];
        }
        else if(strcmp(argv[i], "list") == 0){
            func_indicator = 4;
            i++;
            archive_filename = argv[i];
        }
        else{
            printf("Error : input valid argument\n");
            exit(1);
        }
    }

    // 실제 동작을 수행하는 부분
    if(func_indicator == 0){ // pack
        pack(archive_filename, directory);
    }
    else if(func_indicator == 1){ // unpack
        unpack(archive_filename, directory);
    }
    else if(func_indicator == 2){ // del
        del_file(archive_filename, target_filename);
    }
    else if(func_indicator == 3){ // add
        add_file(archive_filename, target_filename);
    }
    else if(func_indicator == 4){ // list
        list(archive_filename);
    }
    
    return 0;
}

void pack(char* archive_filename, char* src_directory){
    DIR *dir;
    struct dirent *entry;
    char dirname[100] = "./";
    strcat(dirname, src_directory);

    //if(DEBUG) printf("%s dir\n", src_directory);
    //if(DEBUG) printf("%s dir\n", dirname);

    // 디렉토리를 연다. 해당 디렉토리가 없으면, 프로그램을 종료한다.
    dir = opendir(dirname);
    if(dir == NULL){
        printf("Error : invalid source directory\n");
        exit(1);
    }


    // 경과 확인을 위해 추가함
    printf("source directory [%s] opened.\n", src_directory);

    // 압축해야 할 파일들의 개수를 파악하고, 이름들을 각각 파악하자.
    unsigned int file_num = 0;
    while((entry = readdir(dir)) != NULL){
        if(strcmp(entry->d_name, ".")!=0 && strcmp(entry->d_name, "..")!=0){
            file_num++;
        }
    }
    closedir(dir);

    char* filename[file_num];
    char* filename_write[file_num];
    unsigned int filename_length[file_num];
    unsigned int file_size[file_num];
    unsigned int isBinary[file_num];
    FILE* fd_array[file_num];

    // char*를 동적할당으로 만들어주자
    for(unsigned int i=0;i<file_num;i++){
        filename[i] = (char *)malloc(sizeof(char)*100);
        filename_write[i] = (char *)malloc(sizeof(char)*100);
    }

    
    DIR* dir2;
    dir2 = opendir(dirname);
    unsigned int tempCounter = 0;

    fflush(stdout);

    for(unsigned int i=0;i<file_num+2;i++){
        entry = readdir(dir2);
        //if(DEBUG) printf("%s\n", entry->d_name);
        if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0){
            char* temp_filename = (char *)malloc(sizeof(char)*strlen(dirname));
            strcpy(temp_filename, dirname);
            strcat(temp_filename, "/");
            strcat(temp_filename, entry->d_name);
            filename[tempCounter] = temp_filename;
            filename_write[tempCounter] = entry->d_name;
            filename_length[tempCounter] = strlen(entry->d_name);
            tempCounter++;
        }
    }


    // 압축해야 할 파일들의 file descriptor들을 열고 
    // 파일의 크기 정보를 우선 파악하자.
    for(unsigned int i=0;i<file_num;i++){
        //if(DEBUG) printf("%s\n", filename[i]);
        //if(DEBUG) printf("%s\n", filename_write[i]);
        //if(DEBUG) printf("%d\n", filename_length[i]);
        
        fd_array[i] = fopen(filename[i], "rb");
        if(fd_array[i] == NULL){
            printf("file does not exist or some error occured\n");
            exit(1);
        }
        struct stat sb;
        fstat(fd_array[i]->_fileno, &sb);
        file_size[i] = sb.st_size;
        //if(DEBUG) printf("%d\n", file_size[i]);
        isBinary[i] = isBinaryFile(filename[i]);
    }



    // 그 후 파일의 내용을 archive_filename에 집어넣자.
    FILE* archive_file;
    archive_file = fopen(archive_filename, "wb");
    

    // 파일의 개수를 맨 앞에 4byte로 집어넣자.
    fwrite(&file_num, sizeof(unsigned int), 1, archive_file);
    
    // 파일 이름 크기와 파일의 크기를  4byte로 집어넣자
    fwrite(filename_length, sizeof(unsigned int), file_num, archive_file);
    fwrite(file_size, sizeof(unsigned int), file_num, archive_file);
    fwrite(isBinary, sizeof(unsigned int), file_num, archive_file);


    // 그 다음으로 파일의 이름들을 차례대로 넣어야 한다.
    for(unsigned int i=0;i<file_num;i++){
        fwrite(filename_write[i], sizeof(char), filename_length[i], archive_file);
    }


    // 마지막으로 파일의 내용을 차례대로 저장하자.
    unsigned int buffer_size = BUFFER_SIZE;
    for(unsigned int i=0;i<file_num;i++){
        //if(DEBUG) printf("%s\n", filename_write[i]);
        //if(DEBUG) printf("%d\n", file_size[i]);
        // 파일의 크기를 이용해 몇번의 buffer를 사용해야 하는지 파악
        unsigned int temp_iter = (file_size[i] / buffer_size) + 1;
        // unsigned int temp_bufsize = file_size[i] % buffer_size;
        char* buffer = (char *)malloc(buffer_size);
        // char* buffer2 = (char *)malloc(temp_bufsize);
        
        for(unsigned int j=0;j<temp_iter;j++){
            fread(buffer, 1, buffer_size, fd_array[i]);
            fwrite(buffer, 1, buffer_size, archive_file);
        }
        // 나머지에 한해서는 그 사이즈에 맞는 buffer에 집어넣어서 해결하자.
        /*
        if(temp_bufsize > 0){
            fread(buffer2, 1, temp_bufsize, fd_array[i]);
            fwrite(buffer2, 1, temp_bufsize, archive_file);
        }
        */
        
        free(buffer);
        //free(buffer2);

        // fclose(fd_array[i]);
        // 경과 확인을 위해 추가함
        //printf("%s packed\n", filename[i]);
    }

    // 모든게 완료되면 열었던 것들을 다시 다 닫아야 한다.
    fclose(archive_file);
    closedir(dir2);
    //closedir(dir);
    printf("all files successfully packed in %s.\n", archive_filename);

    // 동적할당 한것들도 다 해제해야 한다.
    for(unsigned int i=0;i<file_num;i++){
        free(filename[i]);
        //free(filename_write[i]);
    }
}

void unpack(char* archive_filename, char* dest_directory){
    DIR *dir;
    char dirname[2] = "./";
    strcat(dirname, dest_directory);


    // 디렉토리를 연다. 해당 디렉토리가 없으면, 해당하는 디렉토리를 만든다
    dir = opendir(dirname);
    if(dir == NULL){
        mkdir(dest_directory, 0755);
    }

    // 경과 확인을 위해 추가함
    printf("dest directory [%s] opened.\n", dest_directory);

    // archive file을 열자
    FILE* archive_file;
    archive_file = fopen(archive_filename, "rb");
    if(archive_file == NULL){
        printf("Error : archive file does not exist");
        exit(1);
    }
    printf("archive file %s opened.\n", archive_filename);

    // archive file에서 정보들을 역으로 취득해봅시다.
    unsigned int file_num;
    fread(&file_num, sizeof(unsigned int), 1, archive_file);
    
    char* filename[file_num];
    for(unsigned int i=0;i<file_num;i++){
        filename[i] = (char *)malloc(sizeof(char)*100);
    }

    unsigned int filename_length[file_num];
    unsigned int file_size[file_num];
    unsigned int isBinary[file_num];
    FILE* fd_array[file_num];
    
    fread(filename_length, sizeof(unsigned int), file_num, archive_file);
    fread(file_size, sizeof(unsigned int), file_num, archive_file);
    fread(isBinary, sizeof(unsigned int), file_num, archive_file);

    for(unsigned int i=0;i<file_num;i++){
        fread(filename[i], sizeof(char), filename_length[i], archive_file);
    }

    // 정보를 역으로 취했으면 이제 이 정보들을 가지고 파일을 읽어서 새로운 파일에 쓰면 될 듯.
    char* tempfilename = (char *)malloc(sizeof(char)*100);
    strcpy(tempfilename, dest_directory);
    strcat(tempfilename, "/");

    unsigned int buffer_size = BUFFER_SIZE;

    for(unsigned int i=0;i<file_num;i++){
        char* tempfilename2 = (char *)malloc(sizeof(char)*100);
        strcpy(tempfilename2, tempfilename);
        strcat(tempfilename2, filename[i]);
        if(isBinary[i] == 1){ // 바이너리로 작성해야 한다.
            fd_array[i] = fopen(tempfilename2, "wb");
        }
        else{
            fd_array[i] = fopen(tempfilename2, "w");
        }
        // 파일의 크기를 이용해 몇번의 buffer를 사용해야 하는지 파악
        unsigned int temp_iter = (file_size[i] / buffer_size) + 1;
        //unsigned int temp_bufsize = file_size[i] % buffer_size+1;
        char* buffer = (char *)malloc(buffer_size);
        //char* buffer2 = (char *)malloc(temp_bufsize);

        for(unsigned int j=0;j<temp_iter;j++){
            fread(buffer, 1, buffer_size, archive_file);
            fwrite(buffer, 1, buffer_size, fd_array[i]);
        }

        /*
        if(temp_bufsize > 0){
            fread(buffer2, 1, temp_bufsize, archive_file);
            fwrite(buffer2, 1, temp_bufsize, fd_array[i]);
        }
        */

        fclose(fd_array[i]);
        free(tempfilename2);
        free(buffer);
        //free(buffer2);
        // 경과 확인을 위해 추가함
        //printf("%s unpacked\n", filename[i]);
    }

    // 모든게 완료되면 열었던 것들을 다시 다 닫아야 한다.
    fclose(archive_file);
    closedir(dir);
    printf("all files successfully unpacked in %s.\n", dest_directory);

    // 동적할당 해제
    for(unsigned int i=0;i<file_num;i++){
        free(filename[i]);
    }
    free(tempfilename);
}

// 그냥 매번 파일을 추가할 때마다 새로운 파일을 만들자. 용량을 낭비하는거 보다는 나을 듯.
void add_file(char* archive_filename, char* target_filename){
    // archive file을 열자
    FILE* archive_file;
    archive_file = fopen(archive_filename, "rb");
    if(archive_file == NULL){
        printf("Error : archive file does not exist");
        exit(1);
    }
    printf("archive file %s opened.\n", archive_filename);


    // archive file에서 정보들을 역으로 취득해봅시다.
    unsigned int file_num;
    fread(&file_num, sizeof(unsigned int), 1, archive_file);
    

    // 파일 하나를 추가해야 하니 하나를 더해서 할당 >> file_num에 그냥 1을 더하자.
    file_num++;
    char* filename[file_num];
    for(unsigned int i=0;i<file_num;i++){
        filename[i] = (char *)malloc(sizeof(char)*100);
    }

    unsigned int filename_length[file_num];
    unsigned int file_size[file_num];
    unsigned int isBinary[file_num];


    fread(filename_length, sizeof(unsigned int), file_num-1, archive_file);
    fread(file_size, sizeof(unsigned int), file_num-1, archive_file);
    fread(isBinary, sizeof(unsigned int), file_num-1, archive_file);


    for(unsigned int i=0;i<file_num;i++){
        fread(filename[i], sizeof(char), filename_length[i], archive_file);
    }


    // 새로이 만들어질 temp file을 만들자.
    char* temp_arch = (char *)malloc(sizeof(char)*(strlen(archive_filename)+5));
    strcpy(temp_arch, archive_filename);
    strcat(temp_arch, "_temp");

    FILE* temp_arch_file;
    temp_arch_file = fopen(temp_arch, "wb");


    // 추가할 파일을 열어서 이것에 해당하는 정보들을 각 list에 집어넣자.
    FILE* addfile;
    addfile = fopen(target_filename, "rb");
    if(addfile == NULL){
        printf("file does not exist or some error occured\n");
        exit(1);
    }
    struct stat sb;
    fstat(addfile->_fileno, &sb);
    file_size[file_num-1] = sb.st_size;
    isBinary[file_num-1] = isBinaryFile(target_filename);
    filename[file_num-1] = target_filename;
    filename_length[file_num-1] = strlen(target_filename);


    // 이제 temp file에 정보를 쓰자
    // 파일의 개수를 맨 앞에 4byte로 집어넣자.
    fwrite(&file_num, sizeof(unsigned int), 1, temp_arch_file);
    
    // 파일 이름 크기와 파일의 크기를  4byte로 집어넣자
    fwrite(filename_length, sizeof(unsigned int), file_num, temp_arch_file);
    fwrite(file_size, sizeof(unsigned int), file_num, temp_arch_file);
    fwrite(isBinary, sizeof(unsigned int), file_num, temp_arch_file);


    // 그 다음으로 파일의 이름들을 차례대로 넣어야 한다.
    for(unsigned int i=0;i<file_num;i++){
        fwrite(filename[i], sizeof(char), filename_length[i], temp_arch_file);
    }


    // 마지막으로 데이터를 쓰자. 우선 arch 속 데이터부터 집어넣자

    // 마지막으로 파일의 내용을 차례대로 저장하자.
    unsigned int buffer_size = BUFFER_SIZE;
    for(unsigned int i=0;i<file_num;i++){
        
        // 파일의 크기를 이용해 몇번의 buffer를 사용해야 하는지 파악
        unsigned int temp_iter = (file_size[i] / buffer_size) + 1;
        // unsigned int temp_bufsize = file_size[i] % buffer_size;
        char* buffer = (char *)malloc(buffer_size);
        // char* buffer2 = (char *)malloc(temp_bufsize);
        
        for(unsigned int j=0;j<temp_iter;j++){
            if(i < file_num - 1){ // 이전 파일의 경우
                fread(buffer, 1, buffer_size, archive_file);
            }
            else{ // 최신 파일의 경우
                fread(buffer, 1, buffer_size, addfile);
            }
            fwrite(buffer, 1, buffer_size, temp_arch_file);
        }
        /*
        // 나머지에 한해서는 그 사이즈에 맞는 buffer에 집어넣어서 해결하자.
        if(temp_bufsize > 0){
            if(i < file_num - 1){ // 이전 파일의 경우
                fread(buffer2, 1, temp_bufsize, archive_file);
            }
            else{ // 최신 파일의 경우
                fread(buffer2, 1, temp_bufsize, addfile);
            }
            fwrite(buffer2, 1, temp_bufsize, temp_arch_file);
        }
        */
        

        free(buffer);
        //free(buffer2);
    }


    // 이제 이전 파일을 닫은 후, 새로운 파일을 이전 파일 이름으로 바꾼다.
    fclose(archive_file);
    remove(archive_filename);
    rename(temp_arch, archive_filename);


    // 모든게 완료되었으면 지워야 한다.
    fclose(addfile);
    fclose(temp_arch_file);
    // 동적할당 해제
    for(unsigned int i=0;i<file_num;i++){
        // free(filename[i]);
    }

    printf("file added\n");
}

// 그냥 매번 파일을 추가할 때마다 새로운 파일을 만들자. 용량을 낭비하는거 보다는 나을 듯.
void del_file(char* archive_filename, char* target_filename){
    // archive file을 열자
    FILE* archive_file;
    archive_file = fopen(archive_filename, "rb");
    if(archive_file == NULL){
        printf("Error : archive file does not exist");
        exit(1);
    }
    printf("archive file %s opened.\n", archive_filename);

    // archive file에서 정보들을 역으로 취득해봅시다.
    unsigned int file_num;
    fread(&file_num, sizeof(unsigned int), 1, archive_file);
    
    char* filename[file_num];
    for(unsigned int i=0;i<file_num;i++){
        filename[i] = (char *)malloc(sizeof(char)*100);
    }

    unsigned int filename_length[file_num];
    unsigned int file_size[file_num];
    unsigned int isBinary[file_num];
    
    fread(filename_length, sizeof(unsigned int), file_num, archive_file);
    fread(file_size, sizeof(unsigned int), file_num, archive_file);
    fread(isBinary, sizeof(unsigned int), file_num, archive_file);

    for(unsigned int i=0;i<file_num;i++){
        fread(filename[i], sizeof(char), filename_length[i], archive_file);
    }

    // 지우고자 하는 파일의 위치를 찾는다. 만약에 없다면, 프로그램을 종료한다.
    int target_index = -1;
    for(unsigned int i=0;i<file_num;i++){
        if(strcmp(filename[i], target_filename) == 0){
            target_index = i;
            break;
        }
    }
    if(target_index<0){
        printf("Error : file does not exist in archive file\n");
        exit(1);
    }

    // 새로이 만들어질 temp file을 만들자.
    char* temp_arch = (char *)malloc(sizeof(char)*(strlen(archive_filename)+5));
    strcpy(temp_arch, archive_filename);
    strcat(temp_arch, "_temp");

    FILE* temp_arch_file;
    temp_arch_file = fopen(temp_arch, "wb");
    
    // 이제 temp file에 정보를 쓰자
    // 파일의 개수를 맨 앞에 4byte로 집어넣자.
    unsigned int save_filenum = file_num;
    save_filenum--;
    fwrite(&save_filenum, sizeof(unsigned int), 1, temp_arch_file);
    
    unsigned int target_index_unsigned = target_index;
    // 파일 이름 크기와 파일의 크기를  4byte로 집어넣자
    for(unsigned int i=0;i<file_num;i++){
        if(i != target_index_unsigned){
            fwrite(&filename_length[i], sizeof(unsigned int), 1, temp_arch_file);
        }
    }
    for(unsigned int i=0;i<file_num;i++){
        if(i != target_index_unsigned){
            fwrite(&file_size[i], sizeof(unsigned int), 1, temp_arch_file);
        }
    }
    for(unsigned int i=0;i<file_num;i++){
        if(i != target_index_unsigned){
            fwrite(&isBinary[i], sizeof(unsigned int), 1, temp_arch_file);
        }
    }

    // 그 다음으로 파일의 이름들을 차례대로 넣어야 한다.
    for(unsigned int i=0;i<file_num;i++){
        if(i != target_index_unsigned){
            fwrite(filename[i], sizeof(char), filename_length[i], temp_arch_file);
        }
    }

    // 마지막으로, temp file에 이전 archive_file의 내용들을 집어넣자.
    // 마지막으로 파일의 내용을 차례대로 저장하자.
    unsigned int buffer_size = BUFFER_SIZE;
    for(unsigned int i=0;i<file_num;i++){
        // 파일의 크기를 이용해 몇번의 buffer를 사용해야 하는지 파악
        unsigned int temp_iter = (file_size[i] / buffer_size)+1;
        //unsigned int temp_bufsize = file_size[i] % buffer_size;
        char* buffer = (char *)malloc(buffer_size);
        //char* buffer2 = (char *)malloc(temp_bufsize);
        
        for(unsigned int j=0;j<temp_iter;j++){
            if(i != target_index_unsigned){ // 추가해서는 안되는 index이면 추가하면 안된다.
                fread(buffer, 1, buffer_size, archive_file);
                fwrite(buffer, 1, buffer_size, temp_arch_file);
            }
        }
        /*
        // 나머지에 한해서는 그 사이즈에 맞는 buffer에 집어넣어서 해결하자.
        if(temp_bufsize > 0){
            if(i != target_index_unsigned){ // target_index이면 추가하면 안된다.
                fread(buffer2, 1, temp_bufsize, archive_file);
                fwrite(buffer2, 1, temp_bufsize, temp_arch_file);
            }
        }
        */
        
        free(buffer);
        //free(buffer2);
    }

    // 이제 이전 파일을 닫은 후, 새로운 파일을 이전 파일 이름으로 바꾼다.
    fclose(archive_file);
    remove(archive_filename);
    rename(temp_arch, archive_filename);

    printf("delete file\n");

    // 모든게 완료되었으면 지워야 한다.
    fclose(temp_arch_file);
    // 동적할당 해제
    for(unsigned int i=0;i<file_num;i++){
        free(filename[i]);
    }
}

void list(char* archive_filename){
    // archive file을 열자
    FILE* archive_file;
    archive_file = fopen(archive_filename, "rb");
    if(archive_file == NULL){
        printf("Error : archive file does not exist");
        exit(1);
    }
    printf("archive file %s opened.\n", archive_filename);

    // archive file에서 정보들을 역으로 취득해봅시다.
    unsigned int file_num;
    fread(&file_num, sizeof(unsigned int), 1, archive_file);
    
    char* filename[file_num];
    for(unsigned int i=0;i<file_num;i++){
        filename[i] = (char *)malloc(sizeof(char)*100);
    }

    unsigned int filename_length[file_num];
    unsigned int file_size[file_num];
    unsigned int isBinary[file_num];

    fread(filename_length, sizeof(unsigned int), file_num, archive_file);
    fread(file_size, sizeof(unsigned int), file_num, archive_file);
    fread(isBinary, sizeof(unsigned int), file_num, archive_file);
    
    for(unsigned int i=0;i<file_num;i++){
        fread(filename[i], sizeof(char), filename_length[i], archive_file);
    }

    // 이제 출력을 하자
    printf("%d file(s) exist.\n", file_num);
    for(unsigned int i=0;i<file_num;i++){
        printf("%s\t%dbytes\n", filename[i], file_size[i]);
    }

    // 모든게 완료되면 archive file을 종료
    fclose(archive_file);

    // 동적할당 해제
    for(unsigned int i=0;i<file_num;i++){
        free(filename[i]);
    }
}

int isBinaryFile(char* filename){
    FILE* file = fopen(filename, "rb");
    int isBinary = 0;
    int ch;
    while((ch=fgetc(file))!=EOF){
        if(ch == '\0'){
            isBinary = 1;
            break;
        }
    }
    fclose(file);
    return isBinary;
}