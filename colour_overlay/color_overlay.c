#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//定义结构体
typedef struct{
    int r, g, b;
}PIXEL;

typedef struct{
    int width;
    int height;
    int colorset;
    PIXEL *data; 
}PPM;

//定义变量
const char *read_path1 ="C:\\Users\\Zoe\\OneDrive\\CPP\\PPM\\colour_overlay\\pic1.ppm";//pic1
const char *read_path2 ="C:\\Users\\Zoe\\OneDrive\\CPP\\PPM\\colour_overlay\\pic2.ppm";//pic2
const char *write_path ="C:\\Users\\Zoe\\OneDrive\\CPP\\PPM\\colour_overlay\\pic_overlay.ppm";//同一文件夹下，写输出文件的文件名

PPM inPPM1;
PPM inPPM2;
PPM outPPM;

//检查错误
int ERROR_STATE =0;

enum{
    ERR_FILE_NOT_FOUND = 1,
	ERR_WRONG_FORMAT_HEADER,
	ERR_ILLEGAL_SIZE,
	ERR_FILE_BROKEN,
	ERR_FAILED_TO_WRITE,
};


int checkerror()
{
    return ERROR_STATE;
}

void throwerror(int error)
{
    ERROR_STATE = error;

}

//读取pic1
void read1()
{
    if(freopen(read_path1,"r",stdin)==NULL )
    {
        throwerror(ERR_FILE_NOT_FOUND);
        return;
    }

   char *FARMAT_HEADER = malloc(sizeof(char*));
   scanf("%s",FARMAT_HEADER);
   if(strcmp(FARMAT_HEADER,"P3") != 0)
   {
        throwerror(ERR_WRONG_FORMAT_HEADER);
        return;
   }

   int width,height,colorset;
   scanf("%d%d%d",&width,&height,&colorset);
   if(width<=0 || height <=0 )
   {
        throwerror(ERR_ILLEGAL_SIZE);
        return;
   }

    inPPM1.width = width;
    inPPM1.height = height;
	inPPM1.colorset = colorset;
	inPPM1.data = malloc(sizeof(PIXEL) * width *height);
	int r, g, b;
    for(int y=0; y<height; y++)
    {
        for(int x=0; x<width; x++)
        {
            if (scanf("%d%d%d", &r, &g, &b)==3) {//scanf 返回成功读取的项目数量（0-3）
				inPPM1.data[x + y * width].r = r;//二维数组的一维化存储方式，代表第x列第y行的像素
				inPPM1.data[x + y * width].g = g;
				inPPM1.data[x + y * width].b = b;
			}
            else{
                throwerror(ERR_FILE_BROKEN);
                return;
            }
        }
    }

    fclose(stdin);
}

//读取pic2
void read2()
{
    if(freopen(read_path2,"r",stdin)==NULL )
    {
        throwerror(ERR_FILE_NOT_FOUND);
        return;
    }

   char *FARMAT_HEADER = malloc(sizeof(char*));
   scanf("%s",FARMAT_HEADER);
   if(strcmp(FARMAT_HEADER,"P3") != 0)
   {
        throwerror(ERR_WRONG_FORMAT_HEADER);
        return;
   }

   int width,height,colorset;
   scanf("%d%d%d",&width,&height,&colorset);
   if(width<=0 || height <=0 )
   {
        throwerror(ERR_ILLEGAL_SIZE);
        return;
   }

    inPPM2.width = width;
    inPPM2.height = height;
	inPPM2.colorset = colorset;
	inPPM2.data = malloc(sizeof(PIXEL) * width *height);
	int r, g, b;
    for(int y=0; y<height; y++)
    {
        for(int x=0; x<width; x++)
        {
            if (scanf("%d%d%d", &r, &g, &b)==3) {//scanf 返回成功读取的项目数量（0-3）
				inPPM2.data[x + y * width].r = r;//二维数组的一维化存储方式，代表第x列第y行的像素
				inPPM2.data[x + y * width].g = g;
				inPPM2.data[x + y * width].b = b;
			}
            else{
                throwerror(ERR_FILE_BROKEN);
                return;
            }
        }
    }

    fclose(stdin);
}

//叠加操作
void handle()
{
    //成果图片的长宽以较小值为主,防止数组越界
    outPPM.width = (inPPM1.width<inPPM2.width) ? inPPM1.width : inPPM2.width;
    outPPM.height = (inPPM1.height<inPPM2.height) ? inPPM1.height : inPPM2.height;
    outPPM.colorset = (inPPM1.colorset > inPPM2.colorset) ? inPPM1.colorset : inPPM2.colorset;
    outPPM.data = malloc(sizeof(PIXEL) * outPPM.width * outPPM.height);

    //对每个像素叠加    
   for(int y = 0; y < outPPM.height; y++) {
        for(int x = 0; x < outPPM.width; x++) {
            outPPM.data[x + y * outPPM.width].r = inPPM1.data[x + y * inPPM1.width].r * 0.5 + inPPM2.data[x + y * inPPM2.width].r * 0.5;
            outPPM.data[x + y * outPPM.width].g = inPPM1.data[x + y * inPPM1.width].g * 0.5 + inPPM2.data[x + y * inPPM2.width].g * 0.5; 
            outPPM.data[x + y * outPPM.width].b = inPPM1.data[x + y * inPPM1.width].b * 0.5 + inPPM2.data[x + y * inPPM2.width].b * 0.5; 
        }
    }
}

//把结果写到pic_overlay
void write()
{
    if (checkerror()) {
		return;
	}

	FILE* file = fopen(write_path, "wb");//"wb"二进制
	if (file == NULL) {
		throwerror(ERR_FAILED_TO_WRITE);
		return;
	}

    int flag=0;
    //头部份
    flag |= 0>= fprintf(file, "P3\n"); //flag = flag | (0 >= fprintf(...))
	flag |= 0>= fprintf(file, "%d %d\n", outPPM.width, outPPM.height);
	flag |= 0>= fprintf(file, "%d\n", outPPM.colorset);
    //图像数据部分
    int N=outPPM.width * outPPM.height;
    for(int i=0; i<N; i++)
    {
        flag |= 0>=fprintf(file,"%d\n", outPPM.data[i].r);
        flag |= 0>=fprintf(file,"%d\n", outPPM.data[i].g);
        flag |= 0>=fprintf(file,"%d\n", outPPM.data[i].b);
    }
    if(flag==0){
        throwerror(ERR_FAILED_TO_WRITE);
        return;
    }

    fflush(file);//强制把缓冲区数据写入磁盘
	fclose(file);//关闭文件
    
    free(outPPM.data);
}



int main()
{
    read1();
    read2();
    handle();
    write(); 
    return ERROR_STATE;
}
