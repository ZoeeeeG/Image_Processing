#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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
const char *read_path ="C:\\Users\\Zoe\\OneDrive\\CPP\\PPM\\sobel\\apple.ppm";//写文件地址
const char *write_path ="C:\\Users\\Zoe\\OneDrive\\CPP\\PPM\\sobel\\apple_sobel_gray.ppm";//同一文件夹下，写输出文件的文件名

PPM inPPM;
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

//读取数据
void read()
{
    if(freopen(read_path,"r",stdin)==NULL)
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

    inPPM.width = width;
    inPPM.height = height;
	inPPM.colorset = colorset;
	inPPM.data = malloc(sizeof(PIXEL) * width *height);
	int r, g, b;
    for(int y=0; y<height; y++)
    {
        for(int x=0; x<width; x++)
        {
            if (scanf("%d%d%d", &r, &g, &b)==3) {//scanf 返回成功读取的项目数量（0-3）
				inPPM.data[x + y * width].r = r;//二维数组的一维化存储方式，代表第x列第y行的像素
				inPPM.data[x + y * width].g = g;
				inPPM.data[x + y * width].b = b;
			}
            else{
                throwerror(ERR_FILE_BROKEN);
                return;
            }
        }
    }

    fclose(stdin);
}

//输出数据
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
}


PIXEL BLACK = {0, 0, 0};

//获得（i,j）的rgb
PIXEL getPixel(PPM *source, int x, int y) {
    if (x < 0 || y < 0 || x >= source->width || y >= source->height) {
        return BLACK;
    }
    return source->data[x + y * source->width];
}

int getGx(int i, int j, int x, int y)
{
    int dx = i - x;
    int dy = j - y;

    if(dx==-1 && dy==-1)
        return -1;
    else if(dx==-1 && dy==0)
        return -2;
    else if(dx==-1 && dy==1)
        return -1;
    else if(dx==1 && dy==-1)
        return 1;
    else if(dx==1 && dy==0)
        return 2;
    else if(dx==1 && dy==1)
        return 1;
    else
        return 0;
}

int getGy(int i, int j, int x, int y)
{
    int dx = i - x;
    int dy = j - y;

    if(dx==-1 && dy==-1)
        return -1;
    else if(dx==0 && dy==-1)
        return -1;
    else if(dx==1 && dy==-1)
        return -1;
    else if(dx==-1 && dy==1)
        return 1;
    else if(dx==0 && dy==1)
        return 2;
    else if(dx==1 && dy==1)
        return 1;
    else
        return 0;
}

int getGray(PIXEL p) {
    return (int)((p.r + p.g + p.b)/3);
}

PIXEL sobel(PPM *source, int x, int y)
{
    PIXEL sobel_pixel;
    PIXEL temp;
    PIXEL Gx = {0, 0, 0};
    PIXEL Gy = {0, 0, 0};

    for(int i=x-1; i<=x+1; i++){
        for(int j=y-1; j<=y+1; j++){
            temp = getPixel(source, i, j);
            int gray = getGray(temp);
            int a = getGx(i, j, x, y);
            int b = getGy(i, j, x, y);
            Gx.r += gray * a;
            Gx.g += gray * a;
            Gx.b += gray * a;
            Gy.r += gray * b;
            Gy.g += gray * b;
            Gy.b += gray * b;
        }
    }

    sobel_pixel.r = (int)sqrt(Gx.r*Gx.r + Gy.r*Gy.r);
    sobel_pixel.g = (int)sqrt(Gx.g*Gx.g + Gy.g*Gy.g);
    sobel_pixel.b = (int)sqrt(Gx.b*Gx.b + Gy.b*Gy.b);

    if (sobel_pixel.r > source->colorset) sobel_pixel.r = source->colorset;
    if (sobel_pixel.g > source->colorset) sobel_pixel.g = source->colorset;
    if (sobel_pixel.b > source->colorset) sobel_pixel.b = source->colorset;

    return sobel_pixel;
}



void handle()
{
    int width =  inPPM.width, height = inPPM.height;
    outPPM.width = inPPM.width;
    outPPM.height = inPPM.height;
    outPPM.colorset = inPPM.colorset;
    outPPM.data = malloc(sizeof(PIXEL) * width * height);

    for(int y=0; y<height; y++){
        for(int x=0; x<width; x++){
            outPPM.data[x + y*width] = sobel(&inPPM, x, y);
        }
    }
}

int main()
{
    read();
    handle();
    write();
    return ERROR_STATE;
}
