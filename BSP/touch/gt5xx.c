/**
  ******************************************************************************
  * @file    gt5xx.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   i2c??????????????gt9157оƬ
  ******************************************************************************
  * @attention
  *
  * ʵ??ƽ̨:Ұ??  STM32 F407 ?????? 
  * ??̳    :http://www.firebbs.cn
  * ?Ա?    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
#include "gt5xx.h"
#include "bsp_i2c_touch.h"
#include "bsp_nt35510_lcd.h"
// 4.5????GT5688????????
const uint8_t CTP_CFG_GT5688[] =  {
			0x96,0xE0,0x01,0x56,0x03,0x05,0x35,0x00,0x01,0x00,
			0x00,0x05,0x50,0x3C,0x53,0x11,0x00,0x00,0x22,0x22,
			0x14,0x18,0x1A,0x1D,0x0A,0x04,0x00,0x00,0x00,0x00,
			0x00,0x00,0x53,0x00,0x14,0x00,0x00,0x84,0x00,0x00,
			0x3C,0x19,0x19,0x64,0x1E,0x28,0x88,0x29,0x0A,0x2D,
			0x2F,0x29,0x0C,0x20,0x33,0x60,0x13,0x02,0x24,0x00,
			0x00,0x20,0x3C,0xC0,0x14,0x02,0x00,0x00,0x54,0xAC,
			0x24,0x9C,0x29,0x8C,0x2D,0x80,0x32,0x77,0x37,0x6E,
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0x50,0x3C,
			0xFF,0xFF,0x07,0x00,0x00,0x00,0x02,0x14,0x14,0x03,
			0x04,0x00,0x21,0x64,0x0A,0x00,0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			0x32,0x20,0x50,0x3C,0x3C,0x00,0x00,0x00,0x00,0x00,
			0x0D,0x06,0x0C,0x05,0x0B,0x04,0x0A,0x03,0x09,0x02,
			0xFF,0xFF,0xFF,0xFF,0x00,0x01,0x02,0x03,0x04,0x05,
			0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
			0x10,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0x3C,0x00,0x05,0x1E,0x00,0x02,
			0x2A,0x1E,0x19,0x14,0x02,0x00,0x03,0x0A,0x05,0x00,
			0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xFF,0xFF,0x86,
			0x22,0x03,0x00,0x00,0x33,0x00,0x0F,0x00,0x00,0x00,
			0x50,0x3C,0x50,0x00,0x00,0x00,0x1A,0x64,0x01

};


uint8_t config[GTP_CONFIG_MAX_LENGTH + GTP_ADDR_LENGTH]
                = {GTP_REG_CONFIG_DATA >> 8, GTP_REG_CONFIG_DATA & 0xff};

TOUCH_IC touchIC;								

void* gt_memset(void* dst,int val, size_t count)
{
    void* ret = dst;
    while(count--)
    {
        *(char*)dst = (char)val;
        dst = (char*)dst + 1; //?ƶ?һ???ֽ?
    }
    return ret;
}

int gt_memcmp(const void *s1, const void *s2, int len)
{
    unsigned char *p = (unsigned char *)s1;
    unsigned char *q = (unsigned char *)s2;
    int charCompareStatus = 0;
    //If both pointer pointing same memory block
    if (s1 == s2)
    {
        return charCompareStatus;
    }
    while (len > 0)
    {
        if (*p != *q)
        {  //compare the mismatching character
            charCompareStatus = (*p >*q)?1:-1;
            break;
        }
        len--;
        p++;
        q++;
    }
    return charCompareStatus;
}
void * gt_memcpy(void* dst, const void* src, unsigned int cnt)
{
    char *pszDest = (char *)dst;
    const char *pszSource =( const char*)src;
    if((pszDest!= NULL) && (pszSource!= NULL))
    {
        while(cnt) //till cnt
        {
            //Copy byte by byte
            *(pszDest++)= *(pszSource++);
            --cnt;
        }
    }
    return dst;
}
static int8_t GTP_I2C_Test(void);


/**
  * @brief   ʹ??IIC???????ݴ???
  * @param
  *		@arg i2c_msg:???ݴ????ṹ??
  *		@arg num:???ݴ????ṹ???ĸ???
  * @retval  ???????ɵĴ????ṹ????????????????????0xff
  */
static int I2C_Transfer( struct i2c_msg *msgs,int num)
{
	int im = 0;
	int ret = 0;

	

	for (im = 0; ret == 0 && im != num; im++)
	{
		if ((msgs[im].flags&I2C_M_RD))																//????flag?ж??Ƕ????ݻ???д????
		{
			ret = I2C_ReadBytes(msgs[im].addr, msgs[im].buf, msgs[im].len);		//IIC??ȡ????
		} else
		{
			ret = I2C_WriteBytes(msgs[im].addr,  msgs[im].buf, msgs[im].len);	//IICд??????
		}
	}

	if(ret)
		return ret;

	return im;   													//???????ɵĴ????ṹ????
}

/**
  * @brief   ??IIC?豸?ж?ȡ????
  * @param
  *		@arg client_addr:?豸??ַ
  *		@arg  buf[0~1]: ??ȡ???ݼĴ???????ʼ??ַ
  *		@arg buf[2~len-1]: ?洢?????????ݵĻ???buffer
  *		@arg len:    GTP_ADDR_LENGTH + read bytes count???Ĵ?????ַ????+??ȡ???????ֽ?????
  * @retval  i2c_msgs?????ṹ???ĸ?????2Ϊ?ɹ???????Ϊʧ??
  */
static int32_t GTP_I2C_Read(uint8_t client_addr, uint8_t *buf, int32_t len)
{
    struct i2c_msg msgs[2];
    int32_t ret=-1;
    int32_t retries = 0;

    
    /*һ???????ݵĹ??̿??Է?Ϊ????????????:
     * 1. IIC  д?? Ҫ??ȡ?ļĴ?????ַ
     * 2. IIC  ??ȡ  ????
     * */

    msgs[0].flags = !I2C_M_RD;					//д??
    msgs[0].addr  = client_addr;					//IIC?豸??ַ
    msgs[0].len   = GTP_ADDR_LENGTH;	//?Ĵ?????ַΪ2?ֽ?(??д?????ֽڵ?????)
    msgs[0].buf   = &buf[0];						//buf[0~1]?洢????Ҫ??ȡ?ļĴ?????ַ
    
    msgs[1].flags = I2C_M_RD;					//??ȡ
    msgs[1].addr  = client_addr;					//IIC?豸??ַ
    msgs[1].len   = len - GTP_ADDR_LENGTH;	//Ҫ??ȡ?????ݳ???
    msgs[1].buf   = &buf[GTP_ADDR_LENGTH];	//buf[GTP_ADDR_LENGTH]֮???Ļ??????洢??????????

    while(retries < 5)
    {
        ret = I2C_Transfer( msgs, 2);					//????IIC???ݴ??????̺???????2??????????
        if(ret == 2)break;
        retries++;
    }
    if((retries >= 5))
    {
       
    }
    return ret;
}



/**
  * @brief   ??IIC?豸д??????
  * @param
  *		@arg client_addr:?豸??ַ
  *		@arg  buf[0~1]: Ҫд???????ݼĴ???????ʼ??ַ
  *		@arg buf[2~len-1]: Ҫд????????
  *		@arg len:    GTP_ADDR_LENGTH + write bytes count???Ĵ?????ַ????+д?????????ֽ?????
  * @retval  i2c_msgs?????ṹ???ĸ?????1Ϊ?ɹ???????Ϊʧ??
  */
static int32_t GTP_I2C_Write(uint8_t client_addr,uint8_t *buf,int32_t len)
{
    struct i2c_msg msg;
    int32_t ret = -1;
    int32_t retries = 0;

    
    /*һ??д???ݵĹ???ֻ??Ҫһ??????????:
     * 1. IIC???? д?? ???ݼĴ?????ַ??????
     * */
    msg.flags = !I2C_M_RD;			//д??
    msg.addr  = client_addr;			//???豸??ַ
    msg.len   = len;							//????ֱ?ӵ???(?Ĵ?????ַ????+д?????????ֽ???)
    msg.buf   = buf;						//ֱ??????д?뻺?????е?????(?????˼Ĵ?????ַ)

    while(retries < 5)
    {
        ret = I2C_Transfer(&msg, 1);	//????IIC???ݴ??????̺?????1??????????
        if (ret == 1)break;
        retries++;
    }
    if((retries >= 5))
    {


    }
    return ret;
}



/**
  * @brief   ʹ??IIC??ȡ?ٴ????ݣ??????Ƿ?????
  * @param
  *		@arg client:?豸??ַ
  *		@arg  addr: ?Ĵ?????ַ
  *		@arg rxbuf: ?洢??????????
  *		@arg len:    ??ȡ???ֽ???
  * @retval
  * 	@arg FAIL
  * 	@arg SUCCESS
  */
 int32_t GTP_I2C_Read_dbl_check(uint8_t client_addr, uint16_t addr, uint8_t *rxbuf, int len)
{
    uint8_t buf[16] = {0};
    uint8_t confirm_buf[16] = {0};
    uint8_t retry = 0;
    
    

    while (retry++ < 3)
    {
        gt_memset(buf, 0xAA, 16);
        buf[0] = (uint8_t)(addr >> 8);
        buf[1] = (uint8_t)(addr & 0xFF);
        GTP_I2C_Read(client_addr, buf, len + 2);
        
        gt_memset(confirm_buf, 0xAB, 16);
        confirm_buf[0] = (uint8_t)(addr >> 8);
        confirm_buf[1] = (uint8_t)(addr & 0xFF);
        GTP_I2C_Read(client_addr, confirm_buf, len + 2);

      
        if (!gt_memcmp(buf, confirm_buf, len+2))
        {
            gt_memcpy(rxbuf, confirm_buf+2, len);
            return SUCCESS;
        }
    }    

    return FAIL;
}


/**
  * @brief   ?ر?GT91xx?ж?
  * @param ??
  * @retval ??
  */
void GTP_IRQ_Disable(void)
{

    

    I2C_GTP_IRQDisable();
}

/**
  * @brief   ʹ??GT91xx?ж?
  * @param ??
  * @retval ??
  */
void GTP_IRQ_Enable(void)
{
    
     
	  I2C_GTP_IRQEnable();    
}


/**
  * @brief   ???ڴ????򱨸津?????⵽????
  * @param
  *    @arg     id: ????˳??trackID
  *    @arg     x:  ?????? x ????
  *    @arg     y:  ?????? y ????
  *    @arg     w:  ?????? ??С
  * @retval ??
  */
/*???ڼ?¼????????ʱ(????)????һ?δ???λ?ã?????ֵ??ʾ??һ???޴???????*/
int16_t pre_x[GTP_MAX_TOUCH] ={-1,-1,-1,-1,-1};
int16_t pre_y[GTP_MAX_TOUCH] ={-1,-1,-1,-1,-1};

int16_t lv_x;
int16_t lv_y;
static void GTP_Touch_Down(int32_t id,int32_t x,int32_t y,int32_t w)
{
  
	

	/*ȡx??y??ʼֵ??????Ļ????ֵ*/
	

	
    /* ??????????ť?????ڴ??????? */
    //Touch_Button_Down(x,y); 
	lv_x = y;
	lv_y = 800 - x;

    /*?????????켣?????ڴ??????? */
    //Draw_Trail(pre_x[id],pre_y[id],x,y,&brush);
	
		/************************************/
		/*?ڴ˴??????Լ??Ĵ????㰴??ʱ???????̼???*/
		/* (x,y) ??Ϊ???µĴ????? *************/
		/************************************/
	
		/*prex,prey?????洢??һ?δ?????λ?ã?idΪ?켣????(???㴥??ʱ?ж??켣)*/
    pre_x[id] = x; 
	pre_y[id] = y;
	
	HAL_GPIO_TogglePin(GPIOF,GPIO_PIN_7);
}


/**
  * @brief   ???ڴ????򱨸津???ͷ?
  * @param ?ͷŵ???id??
  * @retval ??
  */
static void GTP_Touch_Up( int32_t id)
{
	

    /*?????????ͷ?,???ڴ???????*/
    //Touch_Button_Up(pre_x[id],pre_y[id]);

		/*****************************************/
		/*?ڴ˴??????Լ??Ĵ??????ͷ?ʱ?Ĵ??????̼???*/
		/* pre_x[id],pre_y[id] ??Ϊ???µ??ͷŵ? ****/
		/*******************************************/	
		/***idΪ?켣????(???㴥??ʱ?ж??켣)********/
	lv_x = -1;
	lv_y = -1;
	
    /*?????ͷţ???pre xy ????Ϊ??*/
	  pre_x[id] = -1;
	  pre_y[id] = -1;		
  
   

}


/**
  * @brief   ????????????????ѯ?????ڴ????жϵ???
  * @param ??
  * @retval ??
  */
static void Goodix_TS_Work_Func(void)
{
    uint8_t  end_cmd[3] = {GTP_READ_COOR_ADDR >> 8, GTP_READ_COOR_ADDR & 0xFF, 0};
    uint8_t  point_data[2 + 1 + 8 * GTP_MAX_TOUCH + 1]={GTP_READ_COOR_ADDR >> 8, GTP_READ_COOR_ADDR & 0xFF};
    uint8_t  touch_num = 0;
    uint8_t  finger = 0;
    static uint16_t pre_touch = 0;
    static uint8_t pre_id[GTP_MAX_TOUCH] = {0};

    uint8_t client_addr=GTP_ADDRESS;
    uint8_t* coor_data = NULL;
    int32_t input_x = 0;
    int32_t input_y = 0;
    int32_t input_w = 0;
    uint8_t id = 0;
 
    int32_t i  = 0;
    int32_t ret = -1;

    

    ret = GTP_I2C_Read(client_addr, point_data, 12);//10?ֽڼĴ?????2?ֽڵ?ַ
    if (ret < 0)
    {
        

        return;
    }
    
    finger = point_data[GTP_ADDR_LENGTH];//״̬?Ĵ???????

    if (finger == 0x00)		//û?????ݣ??˳?
    {
        return;
    }

    if((finger & 0x80) == 0)//?ж?buffer statusλ
    {
        goto exit_work_func;//????δ????????????Ч
    }

    touch_num = finger & 0x0f;//????????
    if (touch_num > GTP_MAX_TOUCH)
    {
        goto exit_work_func;//????????֧?ֵ??????????˳?
    }

    if (touch_num > 1)//??ֹһ????
    {
        uint8_t buf[8 * GTP_MAX_TOUCH] = {(GTP_READ_COOR_ADDR + 10) >> 8, (GTP_READ_COOR_ADDR + 10) & 0xff};

        ret = GTP_I2C_Read(client_addr, buf, 2 + 8 * (touch_num - 1));
        gt_memcpy(&point_data[12], &buf[2], 8 * (touch_num - 1));			//?????????????????ݵ?point_data
    }

    
    
    if (pre_touch>touch_num)				//pre_touch>touch_num,??ʾ?еĵ??ͷ???
    {
        for (i = 0; i < pre_touch; i++)						//һ????һ???㴦??
         {
            uint8_t j;
           for(j=0; j<touch_num; j++)
           {
               coor_data = &point_data[j * 8 + 3];
               id = coor_data[0] & 0x0F;									//track id
              if(pre_id[i] == id)
                break;

              if(j >= touch_num-1)											//??????ǰ????id???Ҳ???pre_id[i]????ʾ???ͷ?
              {
                 GTP_Touch_Up( pre_id[i]);
              }
           }
       }
    }


    if (touch_num)
    {
        for (i = 0; i < touch_num; i++)						//һ????һ???㴦??
        {
            coor_data = &point_data[i * 8 + 3];

            id = coor_data[0] & 0x0F;									//track id
            pre_id[i] = id;

            input_x  = coor_data[1] | (coor_data[2] << 8);	//x????
            input_y  = coor_data[3] | (coor_data[4] << 8);	//y????
            input_w  = coor_data[5] | (coor_data[6] << 8);	//size
        
            {
#if 0
									/*????ɨ??ģʽ????X/Y??ʼ????*/
								switch(LCD_SCAN_MODE)
								{
									case 0:case 7:
										input_y  = LCD_Y_LENGTH - input_y;
										break;
									
									case 2:case 3: 
										input_x  = LCD_X_LENGTH - input_x;
										input_y  = LCD_Y_LENGTH - input_y;
										break;
									
									case 1:case 6:
										input_x  = LCD_X_LENGTH - input_x;
										break;	
									
									default:
									break;
								}
#endif
                GTP_Touch_Down( id, input_x, input_y, input_w);//???ݴ???
            }
        }
    }
    else if (pre_touch)		//touch_ num=0 ??pre_touch??=0
    {
      for(i=0;i<pre_touch;i++)
      {
          GTP_Touch_Up(pre_id[i]);
      }
    }


    pre_touch = touch_num;


exit_work_func:
    {
        ret = GTP_I2C_Write(client_addr, end_cmd, 3);
        if (ret < 0)
        {
            
        }
    }

}


/**
  * @brief   ??????оƬ???¸?λ
  * @param ??
  * @retval ??
  */
 int8_t GTP_Reset_Guitar(void)
{
    
#if 1
    I2C_ResetChip();
    return 0;
#else 		//??????λ
    int8_t ret = -1;
    int8_t retry = 0;
    uint8_t reset_command[3]={(uint8_t)GTP_REG_COMMAND>>8,(uint8_t)GTP_REG_COMMAND&0xFF,2};

    //д?븴λ????
    while(retry++ < 5)
    {
        ret = GTP_I2C_Write(GTP_ADDRESS, reset_command, 3);
        if (ret > 0)
        {
            GTP_INFO("GTP enter sleep!");

            return ret;
        }

    }
    GTP_ERROR("GTP send sleep cmd failed.");
    return ret;
#endif

}



 /**
   * @brief   ????˯??ģʽ
   * @param ??
   * @retval 1Ϊ?ɹ???????Ϊʧ??
   */
//int8_t GTP_Enter_Sleep(void)
//{
//    int8_t ret = -1;
//    int8_t retry = 0;
//    uint8_t reset_comment[3] = {(uint8_t)(GTP_REG_COMMENT >> 8), (uint8_t)GTP_REG_COMMENT&0xFF, 5};//5
//
//    
//
//    while(retry++ < 5)
//    {
//        ret = GTP_I2C_Write(GTP_ADDRESS, reset_comment, 3);
//        if (ret > 0)
//        {
//            GTP_INFO("GTP enter sleep!");
//
//            return ret;
//        }
//
//    }
//    GTP_ERROR("GTP send sleep cmd failed.");
//    return ret;
//}


int8_t GTP_Send_Command(uint8_t command)
{
    int8_t ret = -1;
    int8_t retry = 0;
    uint8_t command_buf[3] = {(uint8_t)(GTP_REG_COMMAND >> 8), (uint8_t)GTP_REG_COMMAND&0xFF, GTP_COMMAND_READSTATUS};

    

    while(retry++ < 5)
    {
        ret = GTP_I2C_Write(GTP_ADDRESS, command_buf, 3);
        if (ret > 0)
        {
            

            return ret;
        }

    }
   
    return ret;
}

/**
  * @brief   ???Ѵ?????
  * @param ??
  * @retval 0Ϊ?ɹ???????Ϊʧ??
  */
int8_t GTP_WakeUp_Sleep(void)
{
    uint8_t retry = 0;
    int8_t ret = -1;

    

    while(retry++ < 10)
    {
        ret = GTP_I2C_Test();
        if (ret > 0)
        {
            
            return ret;
        }
        GTP_Reset_Guitar();
    }

  
    return ret;
}

static int32_t GTP_Get_Info(void)
{
    uint8_t opr_buf[10] = {0};
    int32_t ret = 0;

    uint16_t abs_x_max = GTP_MAX_WIDTH;
    uint16_t abs_y_max = GTP_MAX_HEIGHT;
    uint8_t int_trigger_type = GTP_INT_TRIGGER;
        
    opr_buf[0] = (uint8_t)((GTP_REG_CONFIG_DATA+1) >> 8);
    opr_buf[1] = (uint8_t)((GTP_REG_CONFIG_DATA+1) & 0xFF);
    
    ret = GTP_I2C_Read(GTP_ADDRESS, opr_buf, 10);
    if (ret < 0)
    {
        return FAIL;
    }
    
    abs_x_max = (opr_buf[3] << 8) + opr_buf[2];
    abs_y_max = (opr_buf[5] << 8) + opr_buf[4];
		  
		

    opr_buf[0] = (uint8_t)((GTP_REG_CONFIG_DATA+6) >> 8);
    opr_buf[1] = (uint8_t)((GTP_REG_CONFIG_DATA+6) & 0xFF);
    ret = GTP_I2C_Read(GTP_ADDRESS, opr_buf, 3);
    if (ret < 0)
    {
        return FAIL;
    }
    int_trigger_type = opr_buf[2] & 0x03;
    

    
    return SUCCESS;    
}

/*******************************************************
Function:
    Initialize gtp.
Input:
    ts: goodix private data
Output:
    Executive outcomes.
        0: succeed, otherwise: failed
*******************************************************/
 int32_t GTP_Init_Panel(void)
{
    int32_t ret = -1;



    
	
//uint8_t config[GTP_CONFIG_MAX_LENGTH + GTP_ADDR_LENGTH]
//                = {GTP_REG_CONFIG_DATA >> 8, GTP_REG_CONFIG_DATA & 0xff};
	
    I2C_Touch_Init();

    ret = GTP_I2C_Test();
    if (ret < 0)
    {
        
				return ret;
    } 
		
		//??ȡ????IC???ͺ?
    GTP_Read_Version(); 
    
#if UPDATE_CONFIG
    
    config = (uint8_t *)malloc (GTP_CONFIG_MAX_LENGTH + GTP_ADDR_LENGTH);

		config[0] = GTP_REG_CONFIG_DATA >> 8;
		config[1] =  GTP_REG_CONFIG_DATA & 0xff;
		
		//????IC???ͺ?ָ????ͬ??????
    	if(touchIC == GT5688)
		{
			cfg_info =  CTP_CFG_GT5688; //ָ???Ĵ???????
			cfg_info_len = CFG_GROUP_LEN(CTP_CFG_GT5688);//???????ñ??Ĵ?С
		}
		
    memset(&config[GTP_ADDR_LENGTH], 0, GTP_CONFIG_MAX_LENGTH);
    memcpy(&config[GTP_ADDR_LENGTH], cfg_info, cfg_info_len);
		

		cfg_num = cfg_info_len;
		
		GTP_DEBUG("cfg_info_len = %d ",cfg_info_len);
		GTP_DEBUG("cfg_num = %d ",cfg_num);
		GTP_DEBUG_ARRAY(config,6);
		
		/*????LCD??ɨ?跽?????÷ֱ???*/
		config[GTP_ADDR_LENGTH+1] = LCD_X_LENGTH & 0xFF;
		config[GTP_ADDR_LENGTH+2] = LCD_X_LENGTH >> 8;
		config[GTP_ADDR_LENGTH+3] = LCD_Y_LENGTH & 0xFF;
		config[GTP_ADDR_LENGTH+4] = LCD_Y_LENGTH >> 8;
		
		/*????ɨ??ģʽ????X2Y????*/
		switch(LCD_SCAN_MODE)
		{
			case 0:case 2:case 4: case 6:
				config[GTP_ADDR_LENGTH+6] &= ~(X2Y_LOC);
				break;
			
			case 1:case 3:case 5: case 7:
				config[GTP_ADDR_LENGTH+6] |= (X2Y_LOC);
				break;		
		}

    //????Ҫд??checksum?Ĵ?????ֵ
    check_sum = 0;
		
		for (i = GTP_ADDR_LENGTH; i < (cfg_num+GTP_ADDR_LENGTH -3); i += 2) 
		{
			check_sum += (config[i] << 8) + config[i + 1];
		}
		
		check_sum = 0 - check_sum;
		GTP_DEBUG("Config checksum: 0x%04X", check_sum);
		//????checksum
		config[(cfg_num+GTP_ADDR_LENGTH -3)] = (check_sum >> 8) & 0xFF;
		config[(cfg_num+GTP_ADDR_LENGTH -2)] = check_sum & 0xFF;
		config[(cfg_num+GTP_ADDR_LENGTH -1)] = 0x01;
		

    //д????????Ϣ
    for (retry = 0; retry < 5; retry++)
    {
        ret = GTP_I2C_Write(GTP_ADDRESS, config , cfg_num + GTP_ADDR_LENGTH+2);
        if (ret > 0)
        {
            break;
        }
    }
    Delay(0xfffff);				//?ӳٵȴ?оƬ????
		

		
#if 0	//????д???????ݣ??????Ƿ?????д??
    //????????????????д?????Ƿ???ͬ
	{
    	    uint16_t i;
    	    uint8_t buf[300];
    	     buf[0] = config[0];
    	     buf[1] =config[1];    //?Ĵ?????ַ

    	    

    	    ret = GTP_I2C_Read(GTP_ADDRESS, buf, sizeof(buf));
			   
					GTP_DEBUG("read ");

					GTP_DEBUG_ARRAY(buf,cfg_num);
		
			    GTP_DEBUG("write ");

					GTP_DEBUG_ARRAY(config,cfg_num);

					//???ԱȰ汾??
    	    for(i=1;i<cfg_num+GTP_ADDR_LENGTH-3;i++)
    	    {

    	    	if(config[i] != buf[i])
    	    	{
    	    		GTP_ERROR("Config fail ! i = %d ",i);
							free(config);
    	    		return -1;
    	    	}
    	    }
    	    if(i==cfg_num+GTP_ADDR_LENGTH-3)
	    		GTP_DEBUG("Config success ! i = %d ",i);
	}
#endif
	free(config);
#endif
	 /*ʹ???жϣ????????ܼ??ⴥ??????*/
		//I2C_GTP_IRQEnable();
	
    GTP_Get_Info();

    return 0;
}


/*******************************************************
Function:
    Read chip version.
Input:
    client:  i2c device
    version: buffer to keep ic firmware version
Output:
    read operation return.
        2: succeed, otherwise: failed
*******************************************************/
int32_t GTP_Read_Version(void)
{
    int32_t ret = -1;
    uint8_t buf[8] = {GTP_REG_VERSION >> 8, GTP_REG_VERSION & 0xff};    //?Ĵ?????ַ

    

    ret = GTP_I2C_Read(GTP_ADDRESS, buf, sizeof(buf));
    if (ret < 0)
    {
        
        return ret;
    }
    if (buf[2] == '5')
    {
        
				
				//GT5688оƬ
				if(buf[2] == '5' && buf[3] == '6' && buf[4] == '8'&& buf[5] == '8')
					touchIC = GT5688;
    }        
    else if (buf[5] == 0x00)
    {
        
				
				//GT911оƬ
				if(buf[2] == '9' && buf[3] == '1' && buf[4] == '1')
					touchIC = GT911;
    }
    else
    {
      
				
				//GT9157оƬ
				if(buf[2] == '9' && buf[3] == '1' && buf[4] == '5' && buf[5] == '7')
					touchIC = GT9157;
		}
    return ret;
}

/*******************************************************
Function:
    I2c test Function.
Input:
    client:i2c client.
Output:
    Executive outcomes.
        2: succeed, otherwise failed.
*******************************************************/
static int8_t GTP_I2C_Test( void)
{
    uint8_t test[3] = {GTP_REG_CONFIG_DATA >> 8, GTP_REG_CONFIG_DATA & 0xff};
    uint8_t retry = 0;
    int8_t ret = -1;

    
  
    while(retry++ < 5)
    {
        ret = GTP_I2C_Read(GTP_ADDRESS, test, 3);
        if (ret > 0)
        {
            return ret;
        }
      
    }
    return ret;
}

//???⵽?????ж?ʱ???ã?
void GTP_TouchProcess(void)
{
  
  Goodix_TS_Work_Func();

}






//MODULE_DESCRIPTION("GTP Series Driver");
//MODULE_LICENSE("GPL");
