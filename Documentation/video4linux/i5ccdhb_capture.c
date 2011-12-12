/*
 * Video Capture from I5CCDHB
 *
 *  This program can be used and distributed without restrictions.
 *
 * Media Controller Configuration from:
 * http://processors.wiki.ti.com/index.php/Writing_V4L2_Media_Controller_Applications_on_Dm36x_Video_Capture
 * 
 * V4L2 Grabbing from:
 * http://v4l2spec.bytesex.org/spec/capture-example.html
 *
 */


#define NUM_BUFS 3

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <getopt.h>             /* getopt_long() */

#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <stdint.h>

#include <asm/types.h>          /* for videodev2.h */

#include <linux/videodev2.h>
#include <linux/media.h>
#include <linux/v4l2-subdev.h>
#include <arpa/inet.h>

#define CLEAR(x) memset (&(x), 0, sizeof (x))

#define CODE V4L2_MBUS_FMT_Y12_1X12

#define I5CCDHB_WIDTH 784
#define I5CCDHB_HEIGHT 524


#define LINES_PER_FRAME I5CCDHB_HEIGHT 
#define PIXELS_PER_LINE I5CCDHB_WIDTH/*784*/  /*768*/
#define PBM_DATA_MAX_SIZE (5*LINES_PER_FRAME*PIXELS_PER_LINE+200)

/***** Media Controller Data **********/
#define E_CCDC_NAME		"OMAP3 ISP CCDC"
#define E_VIDEO_CCDC_OUT_NAME	"OMAP3 ISP CCDC output"
#define E_I5CCDHB_NAME	"I5CCDHB Sensor"
#define E_CCP2_NAME  "OMAP3 ISP CCP2"
#define P_I5CCDHB   0  
#define P_CCP2_SOURCE  1  
#define P_CCDC_SINK	0 /* sink pad of ccdc */
#define P_CCDC_SOURCE	1 /* source pad which connects video node */
#define P_VIDEO		0 /* only one input pad for video node */

typedef enum {
	IO_METHOD_READ,
	IO_METHOD_MMAP,
	IO_METHOD_USERPTR,
} io_method;

struct buffer {
  void *                  start;
  size_t                  length;
};

static char *           dev_name        = NULL;
static io_method	io		= IO_METHOD_MMAP;
static int              fd              = -1;
struct buffer *         buffers         = NULL;
static unsigned int     n_buffers       = 0;

static void
errno_exit                      (const char *           s)
{
  fprintf (stderr, "%s error %d, %s\n",
           s, errno, strerror (errno));

  exit (EXIT_FAILURE);
}

static int
xioctl                          (int                    fd,
                                 int                    request,
                                 void *                 arg)
{
  int r;

  do r = ioctl (fd, request, arg);
  while (-1 == r && EINTR == errno);

  return r;
}

int set_links( void )
{
  int media_fd = 0;
          
  static struct media_entity_desc entity[20];
  struct media_link_desc link;
  int ret = 0;
  int index = 0;
  int entities_count = 0;
  
  int E_VIDEO = 0;
  int E_CCDC = 0;
  int E_I5CCDHB = 0;
  int E_CCP2 = 0;

      
  /* 3.open media device */
	media_fd = open("/dev/media0", O_RDWR);
	if (media_fd < 0) {
		printf("%s: Can't open media device %s\n", __func__, "/dev/media0");
		//goto cleanup;
    exit(1);
	}
  sync();
  
  /* 4.enumerate media-entities */
	printf("4.enumerating media entities\n");
	
	while( 0 == ret )
  {
    memset(&entity[index], 0, sizeof(struct media_entity_desc));
    entity[index].id = index | MEDIA_ENT_ID_FLAG_NEXT;
    
		ret = ioctl(media_fd, MEDIA_IOC_ENUM_ENTITIES, &entity[index]);
		if (ret < 0) {
			if (errno == EINVAL)
      {
        
				break;
      }
		}else {
			if (!strcmp(entity[index].name, E_VIDEO_CCDC_OUT_NAME)) {
				E_VIDEO =  entity[index].id;
        //printf("VID...\r\n");
			}
			else if (!strcmp(entity[index].name, E_I5CCDHB_NAME)) {
				E_I5CCDHB =  entity[index].id;
        //printf("I5CCDHB...\r\n");
			}
      else if (!strcmp(entity[index].name, E_CCP2_NAME)) {
				E_CCP2 =  entity[index].id;
        //printf("CCP2...\r\n");
			}
			else if (!strcmp(entity[index].name, E_CCDC_NAME)) {
				E_CCDC =  entity[index].id;
        //printf("CCDC...\r\n");
			}
			//printf("[%x]:%s\n", entity[index].id, entity[index].name);
      sync();
		}
    
		index++;
    
	}
  
	entities_count = index;
  
	printf("total number of entities: %i %i\n", entities_count,index);
  fsync(0);

  /* 6. enable 'i5ccdhb-->ccdc' link */
	printf("6. ENABLEing link [i5ccdhb]----------->[ccdc]\n");
	memset(&link, 0, sizeof(link));

	link.flags |=  MEDIA_LNK_FL_ENABLED;
  link.source.entity = E_I5CCDHB;
	link.source.index = P_I5CCDHB;
	link.source.flags = MEDIA_PAD_FL_SOURCE;

	link.sink.entity = E_CCDC;
	link.sink.index = P_CCDC_SINK;
	link.sink.flags = MEDIA_PAD_FL_SINK;

  fsync(0);
	ret = ioctl(media_fd, MEDIA_IOC_SETUP_LINK, &link);
	if(ret) {
		printf("failed to enable link between i5ccdhb and ccdc: %s\n",strerror(errno));
    
		//goto cleanup;
    exit(1);
	} else
  {
		printf("[i5ccdhb]----------->[ccdc]\tENABLED\n");
  }

  /* 7. enable 'ccdc->memory' link */
	printf("7. ENABLEing link [ccdc]----------->[video_node]\n");
	memset(&link, 0, sizeof(link));

	link.flags |=  MEDIA_LNK_FL_ENABLED;
	link.source.entity = E_CCDC;
	link.source.index = P_CCDC_SOURCE;
	link.source.flags = MEDIA_PAD_FL_SOURCE;

	link.sink.entity = E_VIDEO;
	link.sink.index = P_VIDEO;
	link.sink.flags = MEDIA_PAD_FL_SINK;

	ret = ioctl(media_fd, MEDIA_IOC_SETUP_LINK, &link);
	if(ret) {
		printf("failed to enable link between ccdc and video node\n");
    exit(0);
	} else
		printf("[ccdc]----------->[video_node]\t ENABLED\n");

	printf("**********************************************\n");
  
  close(media_fd);
  
  return 0;
}

int set_format_on_pads( int width , int height )
{
  
  struct v4l2_subdev_format fmt;
  int ret = 0;
  static int i5ccdhb_fd = 0;
  static int ccdc_fd = 0;

  /* 8. set format on pad of i5ccdhb */
	i5ccdhb_fd = open("/dev/v4l-subdev8", O_RDWR);
	if(i5ccdhb_fd == -1) {
		printf("failed to open %s\n", "/dev/v4l-subdev8");
    return -1;
		//goto cleanup;
	}
  
  printf("8. setting format on pad of i5ccdhb entity. . .\n");
  
  memset(&fmt, 0, sizeof(struct v4l2_subdev_format));
  
  fmt.pad = P_I5CCDHB;
  fmt.which = V4L2_SUBDEV_FORMAT_ACTIVE;
  fmt.format.code = CODE;
  fmt.format.width = width;
  fmt.format.height = height;
  fmt.format.field = V4L2_FIELD_NONE;
  
  ret = ioctl(i5ccdhb_fd, VIDIOC_SUBDEV_S_FMT, &fmt);
  if(ret) {
    printf("failed to set format on pad %x\n", fmt.pad);
    return -1;
    //goto cleanup;
  }
  else
  {
    printf("successfully format is set on pad %x\n", fmt.pad);
  }
  /* 9. set format on sink-pad of ccdc */
	ccdc_fd = open("/dev/v4l-subdev2", O_RDWR);
	if(ccdc_fd == -1) {
		printf("failed to open %s\n", "/dev/v4l-subdev2");
		return -2;
	}
  
	/* set format on sink pad of ccdc */
	printf("12. setting format on sink-pad of ccdc entity. . .\n");
	memset(&fmt, 0, sizeof(fmt));

	fmt.pad = P_CCDC_SINK;
	fmt.which = V4L2_SUBDEV_FORMAT_ACTIVE;
	fmt.format.code = CODE;
	fmt.format.width = width;
	fmt.format.height = height;
	fmt.format.field = V4L2_FIELD_NONE;

	ret = ioctl(ccdc_fd, VIDIOC_SUBDEV_S_FMT, &fmt);
	if(ret) {
		printf("failed to set format on pad %x\n", fmt.pad);
		return -3;
	}
	else
  {
		printf("successfully format is set on pad %x\n", fmt.pad);
  }

  /* 13. set format on source-pad of ccdc */
	printf("13. setting format on OF-pad of ccdc entity. . . \n");
	memset(&fmt, 0, sizeof(fmt));

	fmt.pad = P_CCDC_SOURCE;
	fmt.which = V4L2_SUBDEV_FORMAT_ACTIVE;
	fmt.format.code = CODE;
	fmt.format.width = width;
	fmt.format.height = height;
	fmt.format.colorspace = V4L2_COLORSPACE_SMPTE170M;
	fmt.format.field = V4L2_FIELD_NONE;

	ret = ioctl(ccdc_fd, VIDIOC_SUBDEV_S_FMT, &fmt);
	if(ret) {
		printf("failed to set format on pad %x\n", fmt.pad);
		return -4;
	}
	else
  {
		printf("successfully format is set on pad %x\n", fmt.pad);
  }
  
  if( 0 != i5ccdhb_fd )
  {
    close( i5ccdhb_fd );
  }
  
  if( 0 != ccdc_fd )
  {
    close( ccdc_fd );
  }
  
  return 0;
}

static void process_image2 (const void *           p , int len , int frame_num )
{

  static uint8_t au8_pbmData[PBM_DATA_MAX_SIZE];
  uint32_t u32_pbmDataSize = 0;
  uint32_t u32_bytesWritten = 0;
  uint8_t *pu8_pbmDataPtr = &au8_pbmData[0];
  uint16_t *pu16_ptr = (uint16_t*)p;
  
  FILE *file_fd = NULL; 

  int pixels = 0;
  
  char name[] = "framex                       ";

  int min_val = 0xFFFF;
  int max_val = 0;
  
  uint16_t *pu16_filePtr = NULL;
  
  /* Analyze frame */
  
  pu16_ptr = (uint16_t*)p;
  for( pixels = PIXELS_PER_LINE * LINES_PER_FRAME ; 0 != pixels ; pixels-- , pu16_ptr++ ) 
  {
    if( min_val > *pu16_ptr )	{
      min_val = *pu16_ptr;
    }
    if( max_val < *pu16_ptr ){
      max_val = *pu16_ptr;
    }
  }
  
  
  

  if( 0 == max_val )
  {
    //printf("Frame %i invalid!\r\n",frame_num);
    printf("X");
    return;
  }
  printf(".");
  //return ;
  //printf("Min: %d Max: %d\n",min_val,max_val);
  //memset( &au8_pbmData[0] , 0 , PBM_DATA_MAX_SIZE );
  
  sprintf(name , "/frame%i.pgm", frame_num );
  
  /********** Store Image Data in Portable Graymap format *********/
  /********** http://en.wikipedia.org/wiki/Netpbm_format  ********/
  
  u32_pbmDataSize = 0;

  /* Magic Number P5: Binary Portable Graymap */
  u32_bytesWritten = sprintf( (char*)pu8_pbmDataPtr , "P5\n" );
  u32_pbmDataSize += u32_bytesWritten;
  pu8_pbmDataPtr += u32_bytesWritten;

  /* Width, height */
  u32_bytesWritten = sprintf( (char*)pu8_pbmDataPtr , "%d %d\n", PIXELS_PER_LINE , LINES_PER_FRAME );
  u32_pbmDataSize += u32_bytesWritten;
  pu8_pbmDataPtr += u32_bytesWritten;

  /* Maximum value */
  u32_bytesWritten = sprintf( (char*)pu8_pbmDataPtr , "%d\n",  max_val /* (1<<12)-1 */);
  u32_pbmDataSize += u32_bytesWritten;
  pu8_pbmDataPtr += u32_bytesWritten;
  
  /* Image Data */
  u32_pbmDataSize+= PIXELS_PER_LINE * LINES_PER_FRAME * 2;
  
  /* Copy image data, whilst reversing byteorder */
  pu16_ptr = (uint16_t*)p;
  pu16_filePtr = (uint16_t*)pu8_pbmDataPtr;
  
  for( pixels = PIXELS_PER_LINE * LINES_PER_FRAME ; 0 != pixels ; pixels-- ) 
  {
    *pu16_filePtr++ = ntohs(*pu16_ptr++);
  }

  /* Write Data to file */  
  printf("Writing %s...",name);
  file_fd = fopen( name , "w" );
  if( NULL == file_fd ){
    errno_exit("Opening of frame-output-file failed");
  }
  u32_bytesWritten = fwrite( au8_pbmData ,sizeof(uint8_t) , u32_pbmDataSize ,
                             file_fd);
  fclose( file_fd );
  
  if( u32_bytesWritten != u32_pbmDataSize )
  {
    printf("FAILED!\r\n");
  }
  else
  {  
    printf(" done\r\n");
  }
}

static int
read_frame			(int counter)
{
  struct v4l2_buffer buf;
	unsigned int i;

	switch (io) {
    case IO_METHOD_READ:
      if (-1 == read (fd, buffers[0].start, buffers[0].length)) {
        switch (errno) {
          case EAGAIN:
            return 0;

          case EIO:
            /* Could ignore EIO, see spec. */

            /* fall through */

          default:
            errno_exit ("read");
        }
      }

      process_image2 (buffers[buf.index].start , buffers[buf.index].length , counter);
      break;

    case IO_METHOD_MMAP:
      CLEAR (buf);

      buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      buf.memory = V4L2_MEMORY_MMAP;

      if (-1 == xioctl (fd, VIDIOC_DQBUF, &buf)) {
        switch (errno) {
          case EAGAIN:
            return 0;

          case EIO:
            /* Could ignore EIO, see spec. */

            /* fall through */

          default:
            errno_exit ("VIDIOC_DQBUF");
        }
      }

      assert (buf.index < n_buffers);
      //printf("map process\r\n");
                
      process_image2 (buffers[buf.index].start , buffers[buf.index].length , counter);
            
      if (-1 == xioctl (fd, VIDIOC_QBUF, &buf))
        errno_exit ("VIDIOC_QBUF");

      break;

    case IO_METHOD_USERPTR:
      CLEAR (buf);

      buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      buf.memory = V4L2_MEMORY_USERPTR;

      if (-1 == xioctl (fd, VIDIOC_DQBUF, &buf)) {
        switch (errno) {
          case EAGAIN:
            return 0;

          case EIO:
            /* Could ignore EIO, see spec. */

            /* fall through */

          default:
            errno_exit ("VIDIOC_DQBUF");
        }
      }

      for (i = 0; i < n_buffers; ++i)
        if (buf.m.userptr == (unsigned long) buffers[i].start
            && buf.length == buffers[i].length)
          break;

      assert (i < n_buffers);
      
      process_image2 (buffers[buf.index].start , buffers[buf.index].length , counter);
      
      if (-1 == xioctl (fd, VIDIOC_QBUF, &buf))
        errno_exit ("VIDIOC_QBUF");

      break;
	}

	return 1;
}

static void
mainloop                        (void)
{
	unsigned int count;

  count = 0;

  while (count++ < 10 ) {
    for (;;) {
      fd_set fds;
      struct timeval tv;
      int r;

      FD_ZERO (&fds);
      FD_SET (fd, &fds);

      /* Timeout. */
      tv.tv_sec = 2;
      tv.tv_usec = 0;

      r = select (fd + 1, &fds, NULL, NULL, &tv);

      if (-1 == r) {
        if (EINTR == errno)
          continue;

        errno_exit ("select");
      }

      if (0 == r) {
        fprintf (stderr, "select timeout\n");
        exit (EXIT_FAILURE);
      }

			if (read_frame (count))
        break;
	
			/* EAGAIN - continue select loop. */
    }
  }
}

static void
stop_capturing                  (void)
{
  enum v4l2_buf_type type;

	switch (io) {
    case IO_METHOD_READ:
      /* Nothing to do. */
      break;

    case IO_METHOD_MMAP:
    case IO_METHOD_USERPTR:
      type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

      if (-1 == xioctl (fd, VIDIOC_STREAMOFF, &type))
        errno_exit ("VIDIOC_STREAMOFF");

      break;
	}
}

static void
start_capturing                 (void)
{
  unsigned int i;
  enum v4l2_buf_type type;

	switch (io) {
    case IO_METHOD_READ:
      /* Nothing to do. */
      break;

    case IO_METHOD_MMAP:
      for (i = 0; i < n_buffers; ++i) {
        struct v4l2_buffer buf;

        CLEAR (buf);

        buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory      = V4L2_MEMORY_MMAP;
        buf.index       = i;

        if (-1 == xioctl (fd, VIDIOC_QBUF, &buf))
          errno_exit ("VIDIOC_QBUF");
      }
		
      type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

      if (-1 == xioctl (fd, VIDIOC_STREAMON, &type))
        errno_exit ("VIDIOC_STREAMON");

      break;

    case IO_METHOD_USERPTR:
      for (i = 0; i < n_buffers; ++i) {
        struct v4l2_buffer buf;

        CLEAR (buf);

        buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory      = V4L2_MEMORY_USERPTR;
        buf.index       = i;
        buf.m.userptr	= (unsigned long) buffers[i].start;
        buf.length      = buffers[i].length;

        if (-1 == xioctl (fd, VIDIOC_QBUF, &buf))
          errno_exit ("VIDIOC_QBUF");
      }

      type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

      if (-1 == xioctl (fd, VIDIOC_STREAMON, &type))
        errno_exit ("VIDIOC_STREAMON");

      break;
	}
}

static void
uninit_device                   (void)
{
  unsigned int i;

	switch (io) {
    case IO_METHOD_READ:
      free (buffers[0].start);
      break;

    case IO_METHOD_MMAP:
      for (i = 0; i < n_buffers; ++i)
        if (-1 == munmap (buffers[i].start, buffers[i].length))
          errno_exit ("munmap");
      break;

    case IO_METHOD_USERPTR:
      for (i = 0; i < n_buffers; ++i)
        free (buffers[i].start);
      break;
	}

	free (buffers);
}

static void
init_read			(unsigned int		buffer_size)
{
  buffers = calloc (1, sizeof (*buffers));

  if (!buffers) {
    fprintf (stderr, "Out of memory\n");
    exit (EXIT_FAILURE);
  }

	buffers[0].length = buffer_size;
	buffers[0].start = malloc (buffer_size);

	if (!buffers[0].start) {
    fprintf (stderr, "Out of memory\n");
    exit (EXIT_FAILURE);
	}
}

static void
init_mmap			(void)
{
	struct v4l2_requestbuffers req;

  CLEAR (req);

  req.count               = NUM_BUFS;
  req.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  req.memory              = V4L2_MEMORY_MMAP;

	if (-1 == xioctl (fd, VIDIOC_REQBUFS, &req)) {
    if (EINVAL == errno) {
      fprintf (stderr, "%s does not support "
               "memory mapping\n", dev_name);
      exit (EXIT_FAILURE);
    } else {
      errno_exit ("VIDIOC_REQBUFS");
    }
  }

  if (req.count < 2) {
    fprintf (stderr, "Insufficient buffer memory on %s\n",
             dev_name);
    exit (EXIT_FAILURE);
  }

  buffers = calloc (req.count, sizeof (*buffers));

  if (!buffers) {
    fprintf (stderr, "Out of memory\n");
    exit (EXIT_FAILURE);
  }

  for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
    struct v4l2_buffer buf;

    CLEAR (buf);

    buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory      = V4L2_MEMORY_MMAP;
    buf.index       = n_buffers;

    if (-1 == xioctl (fd, VIDIOC_QUERYBUF, &buf))
      errno_exit ("VIDIOC_QUERYBUF");

    buffers[n_buffers].length = buf.length;
    buffers[n_buffers].start =
      mmap (NULL /* start anywhere */,
            buf.length,
            PROT_READ | PROT_WRITE /* required */,
            MAP_SHARED /* recommended */,
            fd, buf.m.offset);

    if (MAP_FAILED == buffers[n_buffers].start)
      errno_exit ("mmap");
  }
}

static void
init_userp			(unsigned int		buffer_size)
{
	struct v4l2_requestbuffers req;
  unsigned int page_size;

  page_size = getpagesize ();
  buffer_size = (buffer_size + page_size - 1) & ~(page_size - 1);

  CLEAR (req);

  req.count               = NUM_BUFS;
  req.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  req.memory              = V4L2_MEMORY_USERPTR;

  if (-1 == xioctl (fd, VIDIOC_REQBUFS, &req)) {
    if (EINVAL == errno) {
      fprintf (stderr, "%s does not support "
               "user pointer i/o\n", dev_name);
      exit (EXIT_FAILURE);
    } else {
      errno_exit ("VIDIOC_REQBUFS");
    }
  }

  buffers = calloc (4, sizeof (*buffers));

  if (!buffers) {
    fprintf (stderr, "Out of memory\n");
    exit (EXIT_FAILURE);
  }

  for (n_buffers = 0; n_buffers < NUM_BUFS; n_buffers++) {
    buffers[n_buffers].length = buffer_size;
    buffers[n_buffers].start = memalign (/* boundary */ page_size,
                                         buffer_size);

    if (!buffers[n_buffers].start) {
      fprintf (stderr, "Out of memory\n");
      exit (EXIT_FAILURE);
		}
  }
}

static void
init_device                     (void)
{
  struct v4l2_capability cap;
  struct v4l2_cropcap cropcap;
  struct v4l2_crop crop;
  struct v4l2_format fmt;
	unsigned int min;

  if (-1 == xioctl (fd, VIDIOC_QUERYCAP, &cap)) {
    if (EINVAL == errno) {
      fprintf (stderr, "%s is no V4L2 device\n",
               dev_name);
      exit (EXIT_FAILURE);
    } else {
      errno_exit ("VIDIOC_QUERYCAP");
    }
  }

  if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
    fprintf (stderr, "%s is no video capture device\n",
             dev_name);
    exit (EXIT_FAILURE);
  }

	switch (io) {
    case IO_METHOD_READ:
      if (!(cap.capabilities & V4L2_CAP_READWRITE)) {
        fprintf (stderr, "%s does not support read i/o\n",
                 dev_name);
        exit (EXIT_FAILURE);
      }

      break;

    case IO_METHOD_MMAP:
    case IO_METHOD_USERPTR:
      if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
        fprintf (stderr, "%s does not support streaming i/o\n",
                 dev_name);
        exit (EXIT_FAILURE);
      }

      break;
	}


  /* Select video input, video standard and tune here. */


	CLEAR (cropcap);

  cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

  if (0 == xioctl (fd, VIDIOC_CROPCAP, &cropcap)) {
    crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    crop.c = cropcap.defrect; /* reset to default */

    if (-1 == xioctl (fd, VIDIOC_S_CROP, &crop)) {
      switch (errno) {
        case EINVAL:
          /* Cropping not supported. */
          break;
        default:
          /* Errors ignored. */
          break;
      }
    }
  } else {	
    /* Errors ignored. */
  }


  CLEAR (fmt);

  fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  fmt.fmt.pix.width       = I5CCDHB_WIDTH; 
  fmt.fmt.pix.height      = I5CCDHB_HEIGHT;
  fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_Y12; //V4L2_PIX_FMT_YUYV;
  fmt.fmt.pix.field       = V4L2_FIELD_NONE; //V4L2_FIELD_INTERLACED;

  if (-1 == xioctl (fd, VIDIOC_S_FMT, &fmt))
    errno_exit ("VIDIOC_S_FMT");

  /* Note VIDIOC_S_FMT may change width and height. */

	/* Buggy driver paranoia. */
	min = fmt.fmt.pix.width * 2;
	if (fmt.fmt.pix.bytesperline < min)
		fmt.fmt.pix.bytesperline = min;
	min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
	if (fmt.fmt.pix.sizeimage < min)
		fmt.fmt.pix.sizeimage = min;

	switch (io) {
    case IO_METHOD_READ:
      init_read (fmt.fmt.pix.sizeimage);
      break;

    case IO_METHOD_MMAP:
      init_mmap ();
      break;

    case IO_METHOD_USERPTR:
      init_userp (fmt.fmt.pix.sizeimage);
      break;
	}
  
  
  set_format_on_pads( I5CCDHB_WIDTH , I5CCDHB_HEIGHT );
  set_links( );
}

static void
close_device                    (void)
{
  if (-1 == close (fd))
    errno_exit ("close");

  fd = -1;
}

static void
open_device                     (void)
{
  struct stat st; 

  if (-1 == stat (dev_name, &st)) {
    fprintf (stderr, "Cannot identify '%s': %d, %s\n",
             dev_name, errno, strerror (errno));
    exit (EXIT_FAILURE);
  }

  if (!S_ISCHR (st.st_mode)) {
    fprintf (stderr, "%s is no device\n", dev_name);
    exit (EXIT_FAILURE);
  }

  fd = open (dev_name, O_RDWR /* required */ | O_NONBLOCK, 0);

  if (-1 == fd) {
    fprintf (stderr, "Cannot open '%s': %d, %s\n",
             dev_name, errno, strerror (errno));
    exit (EXIT_FAILURE);
  }
}

static void
usage                           (FILE *                 fp,
                                 int                    argc,
                                 char **                argv)
{
  fprintf (fp,
           "Usage: %s [options]\n\n"
           "Options:\n"
           "-d | --device name   Video device name [/dev/video2]\n"
           "-h | --help          Print this message\n"
           "-m | --mmap          Use memory mapped buffers\n"
           "-r | --read          Use read() calls\n"
           "-u | --userp         Use application allocated buffers\n"
           "",
           argv[0]);
}

static const char short_options [] = "d:hmru";

static const struct option
long_options [] = {
  { "device",     required_argument,      NULL,           'd' },
  { "help",       no_argument,            NULL,           'h' },
  { "mmap",       no_argument,            NULL,           'm' },
  { "read",       no_argument,            NULL,           'r' },
  { "userp",      no_argument,            NULL,           'u' },
  { 0, 0, 0, 0 }
};

int
main                            (int                    argc,
                                 char **                argv)
{
  dev_name = "/dev/video2";

  for (;;) {
    int index;
    int c;
                
    c = getopt_long (argc, argv,
                     short_options, long_options,
                     &index);

    if (-1 == c)
      break;

    switch (c) {
      case 0: /* getopt_long() flag */
        break;

      case 'd':
        dev_name = optarg;
        break;

      case 'h':
        usage (stdout, argc, argv);
        exit (EXIT_SUCCESS);

      case 'm':
        io = IO_METHOD_MMAP;
        break;

      case 'r':
        io = IO_METHOD_READ;
        break;

      case 'u':
        io = IO_METHOD_USERPTR;
        break;

      default:
        usage (stderr, argc, argv);
        exit (EXIT_FAILURE);
    }
  }

  open_device ();

  init_device ();

  start_capturing ();

  mainloop ();

  stop_capturing ();

  uninit_device ();

  close_device ();

  exit (EXIT_SUCCESS);

  return 0;
}
