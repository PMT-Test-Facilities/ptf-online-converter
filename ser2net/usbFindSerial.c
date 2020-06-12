// usbFindSerial.c : find /dev/ttyUSBNN given a full or abbreviated USB path,
// e.g. usb-0000:00:0b.0-1.1.4 maps into /dev/ttyUSB23
// or abbreviated path usb-1.1.4 maps into /dev/ttyUSB23

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

const char* usbFindSerial1(int debug,const char*usbpath)
{
  FILE *fp;
  const char* keyfile = "/proc/tty/driver/usbserial";
  char buf[256];

  if (debug)
    printf("findUsbSerial: looking for %s\n",usbpath);

  fp = fopen(keyfile,"r");
  if (!fp)
    {
      fprintf(stderr,"usbFindSerial: Cannot read \'%s\', do not know how to map usb paths into /dev/ttyUSBnnn devices.\n",keyfile);
      return NULL;
    }

  while (fgets(buf,sizeof(buf),fp) != NULL)
    {
      char *s, *path;
      int minor = 0;
      char xpath[256];
      //printf("read [%s]\n",buf);

      s = strstr(buf,"path:");

      if (s == NULL)
	continue;

      path = s+5;

      while (*s++)
	if (isspace(*s))
	  break;
      *s = 0;

      strcpy(xpath,"usb-");
      s = path+4;
      while (*s++)
	if (*s == '-')
	  {
	    s++;
	    break;
	  }

      strcat(xpath,s);

      minor = atoi(buf);

      if (debug)
         printf("findUsbSerial: minor %d, path \'%s\', short path \'%s\', compare to \'%s\'\n", minor, path, xpath, usbpath);

      if (strcmp(path,usbpath) == 0 ||
	  strcmp(xpath,usbpath) == 0)
	{
	  FILE *ff;
	  static char dev[256];
	  sprintf(dev, "/dev/ttyUSB%d", minor);

	  ff = fopen(dev,"r");
	  if (ff)
	    {
	      fclose(ff);
	      fclose(fp);
	      return dev;
	    }
	}

    }

  fclose(fp);

  return NULL;
}

const char* usbFindSerial(int debug,const char*usbpath)
{
  FILE *fp;
  const char* keyfile = "find /sys/devices -name \'*ttyUSB*\' -print";
  char buf[256];

  if (usbpath)
     if (debug)
        printf("findUsbSerial: looking for %s\n",usbpath);

  if (!usbpath)
     printf("Available USB serial devices:\n");

  fp = popen(keyfile,"r");
  if (!fp)
    {
      fprintf(stderr,"usbFindSerial: Cannot popen \'%s\', do not know how to map usb paths into /dev/ttyUSBnnn devices.\n", keyfile);
      return NULL;
    }

  if (usbpath)
     usbpath += 4;

  while (fgets(buf,sizeof(buf),fp) != NULL)
    {
      int i;

      //printf("read [%s]\n",buf);

      for (i=0; buf[i]; i++)
         if (buf[i]==':')
            buf[i]='-';
         else if (buf[i]=='\n')
            buf[i]=0;


      if (usbpath)
         if (debug)
            printf("findUsbSerial: path \'%s\' compare to \'%s\'\n", buf, usbpath);

      if (!usbpath)
         {
            printf("%s\n", buf);
            continue;
         }

      if (strstr(buf, usbpath))
	{
          char *s = strstr(buf, "ttyUSB");

          if (s)
             {
                static char dev[256];
                sprintf(dev, "/dev/%s", s);

                FILE *ff;

                ff = fopen(dev,"r");
                if (ff)
                   {
                      fclose(ff);
                      fclose(fp);
                      return dev;
                   }
             }
        }
    }

  fclose(fp);

  return NULL;
}

//#define MAIN 1

#ifdef MAIN
int main(int argc,char*argv[])
{
  //const char* path = "usb-0000:00:0b.0-1.1.6";
  const char* path = "usb-1.1.6";
  printf("device for %s is %s\n",path,usbFindSerial(1,path));
  return 0;
}
#endif

// end
