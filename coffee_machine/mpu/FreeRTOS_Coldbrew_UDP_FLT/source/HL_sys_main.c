#include "HL_sys_common.h"
#include "HL_system.h"
#include "HL_sci.h"
#include "HL_gio.h"
#include "HL_het.h"
#include "HL_adc.h"
#include "HL_esm.h"
#include "HL_etpwm.h"
#include "HL_reg_het.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
/* USER CODE BEGIN (1) */
/* Include FreeRTOS scheduler files */
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "os_task.h"
#include "os_timer.h"
#include "os_semphr.h"

#include "lwip/err.h"
#include "lwip/pbuf.h"
#include "ipv4/lwip/ip_addr.h"
#include "lwip/udp.h"
#define remote_receive gioPORTA,7
#define top_drop gioPORTB,2
// ir
volatile char remote_now=33; //default : '!'
volatile int decode=0;
void decode_ir(void);

// step motor
volatile int step;

//weight sensor
 volatile long weight,weight_zero;
// drop 10ms마다 1번 검사했더니 10번에 한번정도 씹히고 잘 되는듯. -> 씹히는거 gap avg 380ms 찍히는 이상한 애랑 관려있는듯.
 volatile uint8 drop_before;
 volatile uint32 drop_time_counter;
 uint32 drop_count=0, drop_time_avg;
 volatile uint32 drop_time_array[5];
 volatile uint32 drop_sens_timing=0;

//temperature
volatile uint32 value;
volatile float temp;
adcData_t adc_data[1];

// valuable valuables
uint32 time_gap_set=300;
double time_left=0;
volatile uint32 ontime;
volatile long cnt;
volatile uint32 udp_delay=10;
volatile uint32 weight_delay = 10;

#define TSIZE 10
uint8 text[TSIZE]={'I','R',' ','N','O','W','=','X','\n','\r'};
char buf[256]={0};
unsigned int buf_len;
uint8 bitcount[122]={0};
uint8 letter[12]={0};

/************************************** UDP ****************************/
struct udp_pcb *pcb;
char msg[]= "udp testtesttesttesttest\r\n";
struct pbuf *p;
/********************************************/
void sci_display(sciBASE_t *sci, uint8 *txt, uint32 len)
{
    while(len--)
    {
        while((sciREG1->FLR & 0x4) == 4)
            ;
        sciSendByte(sciREG1, *txt++);
    }
}

void delay_us(uint32 time){
    int i=28*time;
    while(i--)
        ;
}

void delay_ms(uint32 time){
    int i=27778*time;
    while(i--)
        ;
}

void wait(uint32 delay)
{
    int i;
    for(i = 0; i < delay; i++)
        ;
}

void wait_66us(uint32 delay){
    delay*=5000;//50us
    hetRAM1->Instruction[0].Data=0; // hetResetTimestamp()와 상동
    while(((hetRAM1->Instruction[0].Data)>>7)<delay)
        ;
}

void wait_us(uint32 delay){
    delay*=75;//us
    hetRAM1->Instruction[0].Data=0; // hetResetTimestamp()와 상동
    while(((hetRAM1->Instruction[0].Data)>>7)<delay)
        ;
}

/*
void wait2_66us(uint32 delay){
    delay*=5000;//us
    hetRAM2->Instruction[0].Data=0; // hetResetTimestamp()와 상동
    while(((hetRAM1->Instruction[0].Data)>>7)<delay)
        ;
}
*/
void lcd_command(unsigned char);
void lcd_char(unsigned char);
void lcd_string(char *);
void lcd_init(void);

void remote_data_calc(void);


#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

void udpTask(void *pvParameters);
extern void vApplicationStackOverflowHook(xTaskHandle *pxTask, signed char *pcTaskName );
extern void EMAC_LwIP_Main (uint8_t * emacAddress);

void udp_echo_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, struct ip_addr *addr, u16_t port);

SemaphoreHandle_t xSemaphore = NULL;


xTaskHandle xTask1;
QueueHandle_t sem1=NULL;


/* Define Task Handles */
xTaskHandle xTask1Handle;
xTaskHandle xTask2Handle;
xTaskHandle xTask3Handle;
xTaskHandle xTask4Handle;
/* USER CODE END */

/** @fn void main(void)
*   @brief Application main function
*   @note This function is empty by default.
*
*   This function is called after startup.
*   The user can use this function to implement the application.
*/

/* USER CODE BEGIN (2) */
/* USER CODE END */

uint8   emacAddress[6U] =   {0x00U, 0x08U, 0xeeU, 0x03U, 0xa6U, 0x6cU};
uint32  emacPhyAddress  =   1U;

void vTask1(void *pvParameters) // remote data calc + sci display   with semaphore
{
    for(;;)
    {
//        if(xSemaphoreTake(sem1, (TickType_t)0x01)==pdTRUE){
        if(xSemaphoreTake(sem1, (TickType_t)0x10)==pdTRUE)
        {

        gioSetBit(gioPORTB,3,0);
        lcd_char(0x80);             //  first line
        lcd_string("remote :   ");
        lcd_char(remote_now);
        //lcd_char((remote_now%10));

        lcd_string("   ");
        gioSetBit(gioPORTB,3,0);
        lcd_char(0x88);             //  first line. 0x8f : last word. 0x88 9번째
        gioSetBit(gioPORTB,3,1);
        delay_ms(1);
        gioSetBit(gioPORTB,3,0);
        lcd_char(0xc0);             // second line
        lcd_string("w:    ");

        lcd_char((weight/1000000)%10+48);
        lcd_char((weight/100000)%10+48);
        lcd_char((weight/10000)%10+48);
        lcd_char((weight/1000)%10+48);
        lcd_char((weight/100)%10+48);
        lcd_char((weight/10)%10+48);
        lcd_char((weight%10)+48);
        lcd_char(223);  //degree display
        lcd_string("C ");

        xSemaphoreGive(sem1);

#if 0

            if(decode==1)
            {
                remote_data_calc();
            }
            //delay_4us(2);


#if 0
            sprintf(buf,"remote data calc finished \n\r\0");
            buf_len = strlen(buf);
            sci_display(sciREG1, (uint8 *)buf, buf_len);
#endif
            if(remote_now<10)
                remote_now+=48;
/* 문제점 : 여기 주석 처리부분은 활성화하면 꼭 맛탱이가 간다. task 루프 안 돌고 여기서 멈춘다. <- remote_now 비교해서 led켜는건잘됨
*/          /*
            sprintf(buf,"ir_now : %d\n\r\0",remote_now);
            buf_len = strlen(buf);
            sci_display(sciREG1, (uint8 *)buf, buf_len);
            */
/*     위의 문제점 아래 3줄로 해결했다. -> uint8로 보내니까 먹통안되고 루프 잘 돈다.
*/


/*    현재 리모콘 버튼 정보 표시
            text[7]=remote_now;
            sci_display(sciREG1, &text[0], TSIZE);
*/
            //wait_66us(10);

            if(remote_now==48)  // 숫자0 버튼 눌리면 USER_LED2 TOGGLE
            gioSetBit(gioPORTB, 7, gioGetBit(gioPORTB, 7) ^ 1);


#endif
//        }
//        xSemaphoreGive(sem1);
        }
        vTaskDelay(1000);
    }
}

// lcd_display
void vTask2(void *pvParameters){
    char data[5]={0};
    int num=0,i=0;
    double fc = 1;
    double rc = 1.0/(2 * M_PI * fc);
    int period = 0.01;
    static double weight_temp = 0;
    unsigned long count=0;
    for(;;)
    {
 //       if(xSemaphoreTake(sem1, (TickType_t)0x01)==pdTRUE){

       // gioSetBit(gioPORTB, 6, gioGetBit(gioPORTB, 6) ^ 1); //user_led_1_toggle
        step=0;   //step motor move. -200 open  +200 close
 //       }
 //       xSemaphoreGive(sem1);

        //taskENTER_CRITICAL();


        if(xSemaphoreTake(sem1, (TickType_t)0x01)==pdTRUE)
        {
            /*
            if(decode==1)
            {
                remote_data_calc();
            }
            */
            //delay_4us(2);

            if(remote_now<10)
                remote_now+=48;

        /*
            while(i<3000000)
            i++;

            gioSetBit(gioPORTB, 6, gioGetBit(gioPORTB, 6) ^ 1);
        }*/ // 이게 1.5초 정도 됬는듯. -> 1줄처리당 대략 0.4~0.5us?
            count =0;
            gioSetBit(hetPORT1,18,1);
            gioSetBit(hetPORT1,2,0);
            while(gioGetBit(hetPORT1,18));

            wait(5);                        //   > 0.1us
            //wait_us(40);
            for(i=0;i<24;i++){
                gioSetBit(hetPORT1,2,1);
                count=count<<1;
                wait(30);
                //wait_us(1);                   //1us
                gioSetBit(hetPORT1,2,0);
                wait(30);
                //wait_us(1);
                if(gioGetBit(hetPORT1,18)) count++;
            }

            gioSetBit(hetPORT1,2,1);
            wait(30);
            //wait_us(1);
            count=count^0x800000;
            weight_temp = count;
            //1-order LPF   M_PI
            weight_temp = (double) (rc * weight_temp + period * count ) / (rc + period);


            weight=(int) weight_temp;
            gioSetBit(hetPORT1,2,0);
            wait(28);
 //       }

       xSemaphoreGive(sem1);
    }

        //taskEXIT_CRITICAL();

        vTaskDelay(weight_delay);
        //vTaskDelay(1000);
    }
}

void vTask3(void *pvParameters){
    int i=0;

    for(;;){

        if(decode==1){

            if(xSemaphoreTake(sem1, (TickType_t)0x10)==pdTRUE)
            {
            while(gioGetBit(gioPORTA,7)==1)
                ;
            for(i=0;i<122;i++)
            {
                //delay_4us(167);  //670us
                wait_us(562);

                if(gioGetBit(gioPORTA,7)==1)
                    bitcount[i]=1+48;
                else
                    bitcount[i]=0+48;
            }

            decode =0;

            remote_data_calc();
            xSemaphoreGive(sem1);
         }
        }

        vTaskDelay(1);

    }

}
int main(void)
{
/* USER CODE BEGIN (3) */
    /*clear the ESM error manually*/
    esmREG->SR1[2] = 0xFFFFFFFFU;
    esmREG->SSR2   = 0xFFFFFFFF;
    esmREG->EKR = 0x0000000A;
    esmREG->EKR = 0x00000000;

    //xSemaphore = xSemaphoreCreateBinary();
    vSemaphoreCreateBinary(sem1);

    /*
    if(xSemaphore == NULL)
    {
        return -1;
    }
     */

    /* Set high end timer GIO port hetPort pin direction to all output */
    gioInit();

    sciInit();
    adcInit();
    hetInit();
    lcd_init();
    /*
    gioSetDirection(gioPORTA, 0b1111111101100111);
    gioSetDirection(gioPORTB, 0b1111111111001000);
    */
    //gioSetDirection(gioPORTA, 0xFFFF);
    //gioSetDirection(gioPORTB, 0xFFFF);

    gioSetDirection(gioPORTA, 0b1111111101101111);
    gioSetDirection(gioPORTB, 0b1111111111001000);

    /* Set high end timer GIO port hetPort pin direction to all output */
    //gioSetDirection(hetPORT1, 0xFFFFFFFF); //HDK uses NHET for LEDs

    //EMAC_LwIP_Main(emacAddress);
    EMAC_LwIP_Main(emacAddress);

    pcb = udp_new();
    //ip_addr_t serv_addr[4]={192,168,0,163};
    //udp_bind(pcb,serv_addr, 7767);
    udp_bind(pcb, IP_ADDR_ANY, 7767);
    udp_recv(pcb, udp_echo_recv, NULL);

    wait(100000);

    gioEnableNotification(remote_receive);
    pwmEnableNotification(hetREG2,pwm0,pwmEND_OF_PERIOD);
    pwmEnableNotification(hetREG2,pwm1,pwmEND_OF_PERIOD);
    //pwmStart(hetRAM2,pwm0);
    _enable_IRQ_interrupt_();

#if 0
    sprintf(buf,"Initializing Success \n\r");
    buf_len = strlen(buf);
    sci_display(sciREG1, (uint8 *) buf, buf_len);
#endif

    /* Create Task 1 */
    //    if (xTaskCreate(vTask1,"Task1", configMINIMAL_STACK_SIZE, NULL, 1, &xTask1Handle) != pdTRUE)
#if 0
    if (xTaskCreate(vTask1,"Task1", configMINIMAL_STACK_SIZE, NULL, 0//  ((configMAX_PRIORITIES-1)|portPRIVILEGE_BIT)
                    , &xTask1Handle) != pdTRUE)
    {
        /* Task could not be created */
        while(1);
    }
#endif

#if 1
    /* Create Task 2 */
    if (xTaskCreate(vTask2,"Task2", configMINIMAL_STACK_SIZE, NULL, 4, &xTask2Handle) != pdTRUE)
    {
        /* Task could not be created */
        while(1);
    }
#endif

#if 0
    if(xTaskCreate(vTask3, "Task3", configMINIMAL_STACK_SIZE, NULL, 5, &xTask3Handle) != pdTRUE)
    {
        while(1);
    }
#endif

#if 1
    if(xTaskCreate(udpTask, "udp", 8 * configMINIMAL_STACK_SIZE, NULL, 8, &xTask4Handle) != pdTRUE)
    {
        while(1);
    }
#endif

    /* Start Scheduler */
    vTaskStartScheduler();

    /* Run forever */
    while(1);
/* USER CODE END */

    return 0;
}


#if 0
void vTask3(void *pvParameters)
{
    etpwmInit();

    for(;;)
    {
        xSemaphoreTake(xSemaphore, (TickType_t) 10);
        if(xSemaphore)
        {
            gioSetBit(gioPORTA, 0, gioGetBit(gioPORTA, 0) ^ 1);
            xSemaphoreGive(xSemaphore);
        }
        vTaskDelay(120);
    }
}
#endif

void udp_echo_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, struct ip_addr *addr, u16_t port)
{
    if (p != NULL)
    {
        gioSetBit(gioPORTB, 6, 1);

        //udp_sendto(pcb, p, IP_ADDR_BROADCAST, 7767); //dest port
        //pbuf_free(p);
        char *rx_pk = p->payload;
        char mmsg[]="a\r\n";
        if(rx_pk[0]!='0')
/*
        sprintf(mmsg,"%s",rx_pk);
        buf_len = strlen(mmsg);
        sci_display(sciREG1, (uint8 *)mmsg, buf_len);
        */
#if 0
        if(rx_pk[0] == 's')
        {
#if 1
            setCNT = rx_pk[1] << 24U |
                     rx_pk[2] << 16U |
                     rx_pk[3] << 8U  |
                     rx_pk[4];
#endif
            setDGR = rx_pk[5] << 24U |
                     rx_pk[6] << 16U |
                     rx_pk[7] << 8U  |
                     rx_pk[8];

#if SCI_DEBUG
        sprintf(vbuf,"%d,%d\n\r",setCNT,setDGR);
#endif
        }
        /* MCU가 준비되서 Ready signal을 전송하면 DSP에서 받고 준비되면 'g'를 보내서 MCU 전체 테스크 동작 시작. */
        else if(rx_pk[0] == 'g')
        {
        }

        /* 다른값이 날라오면 에러 */
        else
        {
#if SCI_DEBUG
            sprintf(vbuf,"UDP Receive ERR\n\r");
#endif
        }

#endif
        pbuf_free(p);

    }
}

void udpTask(void *pvParameters)
{
#if 0
    struct tcp_pcb *pcb;
    err_t err;

    pcb = tcp_new();
    LWIP_ASSERT(("httpd_init: tcp_new failed"), pcb != NULL);
    err = tcp_bind(pcb, IP_ADDR_ANY, HTTPD_SERVER_PORT);
    LWIP_ASSERT(("httpd_init: tcp_bind failed: %s", lwip_strerr(err)), err == ERR_OK);
    pcb = tcp_listen(pcb);
    LWIP_ASSERT(("httpd_init: tcp_listen failed"), pcb != NULL);
    /* initialize callback arg and accept callback */
    tcp_arg(pcb, pcb);
    tcp_accept(pcb, http_accept);
#endif
//    struct udp_pcb *pcb;
//    char msg[] = "udp test-gihwagihwagihwagihwa\r\n";
//    struct pbuf *p;

    err_t err;

#if 0
    xSemaphoreTake(xSemaphore, (TickType_t) 10);
    if(xSemaphore)
    {
        pcb = udp_new();
        err = udp_bind(pcb, IP_ADDR_ANY, 7777);
        udp_recv(pcb, udp_echo_recv, NULL);
        xSemaphoreGive(xSemaphore);
    }
#else
//    pcb = udp_new();
//    udp_bind(pcb, IP_ADDR_ANY, 7767);
#endif
//    udp_recv(pcb,udp_echo_recv, NULL);

    for(;;)
    {

        if(xSemaphoreTake(sem1, (TickType_t)0x01)==pdTRUE)
        {
        //taskENTER_CRITICAL();
            //sprintf(msg,"result : %x %x %x %x %x %x %x %x %x %x %x %x !\n\r\0",letter[0],letter[1],letter[2],letter[3],letter[4],letter[5],letter[6],letter[7],letter[8],letter[9],letter[10],letter[11]);

        sprintf(msg, "temp=%f,weight=%d,remote=%d,drop=%d,t=%d\r\n",temp,weight,remote_now,drop_count,ontime);
        p = pbuf_alloc(PBUF_TRANSPORT, strlen(msg), PBUF_RAM);
        memcpy(p->payload, msg, strlen(msg));
        udp_sendto(pcb, p, IP_ADDR_BROADCAST, 7767);
        pbuf_free(p);

        //q = pbuf_alloc(PBUF_TRANSPORT, strlen(msg), PBUF_RAM);
        //udp_input(pcb,ip_route(&pcb->local_ip));
        //pcb->recv(pcb->recv_arg, pcb, q, ip_current_src_addr(), 7767);

        //strxfrm(q->payload, msg, strlen(msg));
        //udp_recv(pcb, (udp_recv_fn)recv, NULL);


        //strxfrm(p->payload, msg, strlen(msg));
        //pbuf_strstr(q->payload,msg);
        /*
        sprintf(buf,"%s",msg);
         buf_len = strlen(buf);
         sci_display(sciREG1, (uint8 *)buf, buf_len);
    */
         //pbuf_free(q);



        //udp_recv(pcb, p, NULL);
        xSemaphoreGive(sem1);
       }

        //taskEXIT_CRITICAL();

        vTaskDelay(udp_delay);
        //vTaskDelay(200);
    }
}

//void   pwmNotification(hetBASE_t * hetREG,uint32 pwm, uint32 notification);
void pwmNotification(hetBASE_t *port, uint32 pwm,uint32 notification){
    static uint32 time=0;
    static int stp=0;
    static int weight_com_bit_counter=0;
    static int weight_sens_flag=0;
    int i=0,j=0;
    uint8 drop_now=0;
    static unsigned int checker = 0;
    static unsigned long count=0;

    switch(pwm) {

    case 0 :
         //het 타이머 50us주기로 진짜 도나 5초마다 깜박이기로 확인.
        /*
        checker++;
        if((checker%100000)==0){
            gioSetBit(gioPORTB, 6, gioGetBit(gioPORTB, 6) ^ 1);
            checker=0;
        }
        */

#if 0
        if((weight_com_bit_counter==0)&&(weight_sens_flag==1))
        {
            ;
        }

        if((weight_com_bit_counter==0)&&(time%100==0)){

            if(weight_sens_flag==0){
             gioSetBit(hetPORT1,18,1);
             gioSetBit(hetPORT1,2,0);
             weight_sens_flag=1;
             count = 0;
            }
         }

        if(weight_com_bit_counter == 0x01){
             //gioSetBit(hetPORT1,2,1);

             //count = count^0x800000;
             weight= count;
             //wait_us(1);

             weight_com_bit_counter--;

             gioSetBit(hetPORT1,2,0);
             weight_sens_flag=0;


             }

//#if 0
        else if(weight_com_bit_counter>1){
                if(weight_com_bit_counter<49){
                 if((weight_com_bit_counter%2)==0)
                 gioSetBit(hetPORT1, 2, 0);
                 //wait_us(1);
                 else{

                     if(gioGetBit(hetPORT1,18)==1){
                         count++;
                         //checker++;
                         gioSetBit(gioPORTB, 7, 1);
                         if(checker>1)
                             gioSetBit(gioPORTB, 6, 1);
                     //gioSetBit(gioPORTB, 6, 1);
                     }

                     gioSetBit(hetPORT1, 2, 1);
                     count=count<<1;
                 }
                 weight_com_bit_counter--;
                }

                if(weight_com_bit_counter==49){
                 gioSetBit(hetPORT1, 2, 1);
                 count=count<<1;
                 weight_com_bit_counter--;
                }
             }

        if((gioGetBit(hetPORT1,18)==0)&&(weight_sens_flag==1)&&(weight_com_bit_counter==0)){

            weight_com_bit_counter=49;      //24 times
            //wait(3);                        //   > 0.1us
        }

#endif

/*
            gioSetBit(hetPORT1,18,1);
            gioSetBit(hetPORT1,2,0);
            while(gioGetBit(hetPORT1,18));

            wait(3);                        //   > 0.1us

            for(i=0;i<24;i++){
                gioSetBit(hetPORT1,2,1);
                count=count<<1;
                wait_us(1);                   //1us
                gioSetBit(hetPORT1,2,0);
                wait_us(1);
                if(gioGetBit(hetPORT1,18)) count++;
            }

            gioSetBit(hetPORT1,2,1);
            wait_us(1);
            count=count^0x800000;
            weight=count;
            gioSetBit(hetPORT1,2,0);
  */
 //        }
 //        xSemaphoreGive(sem1);
         //vTaskDelay(6000);
//#endif


         //temperature sensing
//        if((time%10)==0){
         adcStartConversion(adcREG1, adcGROUP1);
         while((adcIsConversionComplete(adcREG1, adcGROUP1)) == 0)
                     ;
         adcGetData(adcREG1, adcGROUP1, &adc_data[0]);
         value = adc_data[0].value;
         temp = (float)value/16383;
         temp = 10 -66.875 + (float)(217.75*3.3*temp);
 //       }


         // display sensed
#if 0
         sprintf(buf,"weight calculated : %d, temperature : %f\n\r\0",weight,temp);
         buf_len = strlen(buf);
         sci_display(sciREG1, (uint8 *)buf, buf_len);
#endif
         // user_led_toggle

//         gioSetBit(gioPORTB, 7, gioGetBit(gioPORTB, 7) ^ 1);
        break;

    case 1 :
        if(time > 0xfffffff0){
            time=0; // if 'time' overflows, we need higher class of 'time' to memorize
            ontime=0;
        }
            time++;
            ontime=time;
//        for(j=0;j<1;j++){

            drop_now=gioGetBit(top_drop);
            if((drop_before - drop_now) == 1){
            //if((drop_before==1)&&(drop_now==0)){
                drop_count++;
                drop_time_array[drop_count%5]=time;
                if((drop_count%5)==4){
                    drop_time_avg=(drop_time_array[4]-drop_time_array[0])/4;
                }
            }
            drop_before=drop_now;
            //wait(15000);
 //       }
          //00000011100000000001110000000000001111000000 1초1번드랍일때 이정도로 뜬다.
        // 20ms당 한번 기록할 때는 드랍 카운트가 종종씹힌다.
        /*
        sprintf(buf,"%d", gioGetBit(top_drop));
         buf_len = strlen(buf);
         sci_display(sciREG1, (uint8 *) buf, buf_len);
        */
/*
            if(time%50==0){
            sprintf(buf,"drop_count : %d , drop gap avg = %d ms, time = %d\n\r",drop_count, drop_time_avg*10, time);
            buf_len = strlen(buf);
            sci_display(sciREG1, (uint8 *) buf, buf_len);

            }
*/
        if(step>0){
            if(stp>step){
                step=0;
                stp=0;
/*
                sprintf(buf,"---------step motor finish \n\r\0");
                buf_len = strlen(buf);
                sci_display(sciREG1, (uint8 *)buf, buf_len);
*/
            }
            stp++;
            switch(cnt) {
            case 0: gioSetBit(hetPORT1,12,1);
                    gioSetBit(hetPORT1,14,0);
                    gioSetBit(hetPORT1,30,0);
                    gioSetBit(hetPORT1,16,0);
                    cnt=1;
                    break;
            case 1: gioSetBit(hetPORT1,12,1);
                    gioSetBit(hetPORT1,14,1);
                    gioSetBit(hetPORT1,30,0);
                    gioSetBit(hetPORT1,16,0);
                    cnt=2;
                    break;
            case 2: gioSetBit(hetPORT1,12,0);
                    gioSetBit(hetPORT1,14,1);
                    gioSetBit(hetPORT1,30,0);
                    gioSetBit(hetPORT1,16,0);
                    cnt=3;
                    break;
            case 3: gioSetBit(hetPORT1,12,0);
                    gioSetBit(hetPORT1,14,1);
                    gioSetBit(hetPORT1,30,1);
                    gioSetBit(hetPORT1,16,0);
                    cnt=4;
                    break;
            case 4: gioSetBit(hetPORT1,12,0);
                    gioSetBit(hetPORT1,14,0);
                    gioSetBit(hetPORT1,30,1);
                    gioSetBit(hetPORT1,16,0);
                    cnt=5;
                    break;
            case 5: gioSetBit(hetPORT1,12,0);
                    gioSetBit(hetPORT1,14,0);
                    gioSetBit(hetPORT1,30,1);
                    gioSetBit(hetPORT1,16,1);
                    cnt=6;
                    break;
            case 6: gioSetBit(hetPORT1,12,0);
                    gioSetBit(hetPORT1,14,0);
                    gioSetBit(hetPORT1,30,0);
                    gioSetBit(hetPORT1,16,1);
                    cnt=7;
                    break;
            case 7: gioSetBit(hetPORT1,12,1);
                    gioSetBit(hetPORT1,14,0);
                    gioSetBit(hetPORT1,30,0);
                    gioSetBit(hetPORT1,16,1);
                    cnt=0;
                    break;
            }
        }

        else if(step<0){
            if(stp<step){
                step=0;
                stp=0;
/*
                sprintf(buf,"---------step motor finish \n\r\0");
                buf_len = strlen(buf);
                sci_display(sciREG1, (uint8 *)buf, buf_len);
*/
            }
            stp--;
            switch(cnt) {
            case 0: gioSetBit(hetPORT1,16,1);
                    gioSetBit(hetPORT1,30,0);
                    gioSetBit(hetPORT1,14,0);
                    gioSetBit(hetPORT1,12,0);
                    cnt=1;
                    break;
            case 1: gioSetBit(hetPORT1,16,1);
                    gioSetBit(hetPORT1,30,1);
                    gioSetBit(hetPORT1,14,0);
                    gioSetBit(hetPORT1,12,0);
                    cnt=2;
                    break;
            case 2: gioSetBit(hetPORT1,16,0);
                    gioSetBit(hetPORT1,30,1);
                    gioSetBit(hetPORT1,14,0);
                    gioSetBit(hetPORT1,12,0);
                    cnt=3;
                    break;
            case 3: gioSetBit(hetPORT1,16,0);
                    gioSetBit(hetPORT1,30,1);
                    gioSetBit(hetPORT1,14,1);
                    gioSetBit(hetPORT1,12,0);
                    cnt=4;
                    break;
            case 4: gioSetBit(hetPORT1,16,0);
                    gioSetBit(hetPORT1,30,0);
                    gioSetBit(hetPORT1,14,1);
                    gioSetBit(hetPORT1,12,0);
                    cnt=5;
                    break;
            case 5: gioSetBit(hetPORT1,16,0);
                    gioSetBit(hetPORT1,30,0);
                    gioSetBit(hetPORT1,14,1);
                    gioSetBit(hetPORT1,12,1);
                    cnt=6;
                    break;
            case 6: gioSetBit(hetPORT1,16,0);
                    gioSetBit(hetPORT1,30,0);
                    gioSetBit(hetPORT1,14,0);
                    gioSetBit(hetPORT1,12,1);
                    cnt=7;
                    break;
            case 7: gioSetBit(hetPORT1,16,1);
                    gioSetBit(hetPORT1,30,0);
                    gioSetBit(hetPORT1,14,0);
                    gioSetBit(hetPORT1,12,1);
                    cnt=0;
                    break;
            }
        }
        break;
    }
}

void lcd_command(unsigned char cmmd){

    gioSetBit(gioPORTA,5,((cmmd>>7) & 0x01));
    gioSetBit(gioPORTA,2,((cmmd>>6) & 0x01));
    gioSetBit(gioPORTA,1,((cmmd>>5) & 0x01));
    gioSetBit(gioPORTA,0,((cmmd>>4) & 0x01));

    wait_us(20);
    //delay_us(1);
    gioSetBit(gioPORTA,6,1);
    wait_66us(200);
    //delay_ms(1);
    gioSetBit(gioPORTA,6,0);

}

void lcd_char(unsigned char cmmd)
{
    gioSetBit(gioPORTA,5,((cmmd>>7) & 0x01));
    gioSetBit(gioPORTA,2,((cmmd>>6) & 0x01));
    gioSetBit(gioPORTA,1,((cmmd>>5) & 0x01));
    gioSetBit(gioPORTA,0,((cmmd>>4) & 0x01));


    gioSetBit(gioPORTA,6,1);
    wait_66us(200);
    //delay_ms(1);
    gioSetBit(gioPORTA,6,0);

    wait_us(600);
    //delay_us(200);

    gioSetBit(gioPORTA,5,((cmmd>>3) & 0x01));
    gioSetBit(gioPORTA,2,((cmmd>>2) & 0x01));
    gioSetBit(gioPORTA,1,((cmmd>>1) & 0x01));
    gioSetBit(gioPORTA,0,(cmmd & 0x01));

    gioSetBit(gioPORTA,6,1);
    wait_66us(200);
    //delay_ms(1);
    gioSetBit(gioPORTA,6,0);
    wait_66us(400);
    //delay_ms(2);
}

void lcd_char_variable(unsigned char cmmd)
{
    gioSetBit(gioPORTA,5,((cmmd>>7) & 0x01));
    gioSetBit(gioPORTA,2,((cmmd>>6) & 0x01));
    gioSetBit(gioPORTA,1,((cmmd>>5) & 0x01));
    gioSetBit(gioPORTA,0,((cmmd>>4) & 0x01));


    gioSetBit(gioPORTA,6,1);
    delay_ms(1);
    gioSetBit(gioPORTA,6,0);

    delay_us(200);

    gioSetBit(gioPORTA,5,((cmmd>>3) & 0x01));
    gioSetBit(gioPORTA,2,((cmmd>>2) & 0x01));
    gioSetBit(gioPORTA,1,((cmmd>>1) & 0x01));
    gioSetBit(gioPORTA,0,(cmmd & 0x01));

    gioSetBit(gioPORTA,6,1);
    delay_ms(1);
    gioSetBit(gioPORTA,6,0);
    delay_ms(2);
}


void lcd_string(char *str)
{
    int i=0;
    gioSetBit(gioPORTB,3,1);    //rs=1

    for(i=0;str[i]!=0;i++)
    {
        lcd_char(str[i]);
    }
}

void lcd_init(void){
    char data[5]={0};
    int i=0, num;

    //rw=0, RS=0, E=0
    gioSetBit(gioPORTB,2,0);
    gioSetBit(gioPORTB,3,0);
    gioSetBit(gioPORTA,6,0);
    wait_66us(2000);
    //delay_ms(35);
    //func set
    for(i=0;i<2;i++)
    {
        lcd_command(0x30);
        wait_66us(100);
        //delay_ms(1);
    }
    //lcd_command(0x30);
    //delay_us(150);
    //lcd_command(0x30);
    lcd_command(0x20);

    lcd_char(0x28);
    lcd_char(0x0c);
    lcd_char(0x01);
    lcd_char(0x06);
    lcd_char(0x80);

#if 0   //working. letter weird
    lcd_char(0x28);
    lcd_char(0x0f);     // display on off ctrl.
    lcd_char(0x01);     // clear display
    lcd_char(0x06);     // set cursor move direction and shift
    lcd_char(0x80);     // sets ddram address. ddram data is sent after this.
#endif

#if 0  // not working
    lcd_char(0x80);
    lcd_char(0x10);
    lcd_char(0x70);
#endif

    gioSetBit(gioPORTB,6, (0x00 & (0x10 >>4))); //bit moving is working

    wait_66us(1000);
    //delay_ms(5);

    lcd_string("hello");

    gioSetBit(gioPORTB,3,0);
    lcd_char(0xc0); // go to 2nd line
    lcd_string("HELLO LCD");

    for(i=0;i<5;i++){
    gioSetBit(gioPORTB,3,0);
    lcd_char(0x80);             //  first line
    lcd_string("value : ");

    gioSetBit(gioPORTB,3,0);
    lcd_char(0x88);
    num=7777;

    gioSetBit(gioPORTB,3,1);    //rs=1

    data[0]=(num/10000)%10;
    data[1]=(num/1000)%10;
    data[2]=(num/100)%10;
    data[3]=(num/10)%10;
    data[4]=num%10;

    lcd_char(0x88);             //  first line. 0x8f : last word. 0x88 9번째
    //printf("%d",data[0]);
    gioSetBit(gioPORTB,3,1);    //rs=1
    for(i=0;i<5;i++)
    {
        lcd_char(data[i]+48);
    }

    //lcd_char(48);
    //lcd_char(49);

    gioSetBit(gioPORTB,3,0);
    lcd_char(0xc0);             // second line
    lcd_string("HELLO LCD");
    wait_66us(200);
    //delay_ms(1);
    }
}

// IR REMOTE CONTROLLER CODE

void gioNotification(gioPORT_t *port, uint32 bit)
{
   // _disable_IRQ_interrupt_();

    //gioSetBit(gioPORTB, 6, 1);
 /*
    if(decode==0){
        decode_ir();
    }
*/
//    wait(29000);
    decode = 1;
    //_enable_IRQ_interrupt_();
}

void decode_ir(void){
    int i=0;
    //_disable_IRQ_interrupt_();
    wait(29000);
 //   while(gioGetBit(gioPORTA,7)==1){
 //   }

    for(i=0;i<122;i++)
    {
        //delay_4us(167);  //670us
        wait_us(562);

        if(gioGetBit(gioPORTA,7)==1)
            bitcount[i]=1+48;
        else
            bitcount[i]=0+48;
    }

    decode=1;

 //   sci_display(sciREG1,bitcount,122);
 //   wait(10);
}
void remote_data_calc()
{
    //gioSetPort(gioPORTB, gioGetPort(gioPORTB)^ 0x0000080);
#if 0
    sprintf(buf,"access in remote_data_calc \n\r\0");
    buf_len = strlen(buf);
    sci_display(sciREG1, (uint8 *)buf, buf_len);
#endif
    int i=0;
    while(!((bitcount[i]==48)&&(bitcount[i+1]==49)&&(bitcount[i+2]==49)&&(bitcount[i+3]==49)&&(bitcount[i+4]==48)&&(bitcount[i+5]==49)&&(bitcount[i+6]==49)&&(bitcount[i+7]==49)&&(bitcount[i+8]==48)))
    {
        i++;
        if(i==50)
            break;
    }

/*
    sprintf(buf, "num = %d!\n\r\0",i);
    buf_len = strlen(buf);
    sci_display(sciREG1, (uint8 *)buf, buf_len);
*/
    letter[0]+=bitcount[i+32]*pow(2,3);
    letter[0]+=bitcount[i+33]*pow(2,2);
    letter[0]+=bitcount[i+34]*pow(2,1);
    letter[0]+=bitcount[i+35]*pow(2,0);
    letter[1]+=bitcount[i+36]*pow(2,3);
    letter[1]+=bitcount[i+37]*pow(2,2);
    letter[1]+=bitcount[i+38]*pow(2,1);
    letter[1]+=bitcount[i+39]*pow(2,0);

    letter[2]+=bitcount[i+40]*pow(2,3);
    letter[2]+=bitcount[i+41]*pow(2,2);
    letter[2]+=bitcount[i+42]*pow(2,1);
    letter[2]+=bitcount[i+43]*pow(2,0);
    letter[3]+=bitcount[i+44]*pow(2,3);
    letter[3]+=bitcount[i+45]*pow(2,2);
    letter[3]+=bitcount[i+46]*pow(2,1);
    letter[3]+=bitcount[i+47]*pow(2,0);

    letter[4]+=bitcount[i+48]*pow(2,3);
    letter[4]+=bitcount[i+49]*pow(2,2);
    letter[4]+=bitcount[i+50]*pow(2,1);
    letter[4]+=bitcount[i+51]*pow(2,0);
    letter[5]+=bitcount[i+52]*pow(2,3);
    letter[5]+=bitcount[i+53]*pow(2,2);
    letter[5]+=bitcount[i+54]*pow(2,1);
    letter[5]+=bitcount[i+55]*pow(2,0);

    letter[6]+=bitcount[i+56]*pow(2,3);
    letter[6]+=bitcount[i+57]*pow(2,2);
    letter[6]+=bitcount[i+58]*pow(2,1);
    letter[6]+=bitcount[i+59]*pow(2,0);
    letter[7]+=bitcount[i+60]*pow(2,3);
    letter[7]+=bitcount[i+61]*pow(2,2);
    letter[7]+=bitcount[i+62]*pow(2,1);
    letter[7]+=bitcount[i+63]*pow(2,0);

    letter[8]+=bitcount[i+64]*pow(2,3);
     letter[8]+=bitcount[i+65]*pow(2,2);
     letter[8]+=bitcount[i+66]*pow(2,1);
     letter[8]+=bitcount[i+67]*pow(2,0);
     letter[9]+=bitcount[i+68]*pow(2,3);
     letter[9]+=bitcount[i+69]*pow(2,2);
     letter[9]+=bitcount[i+70]*pow(2,1);
     letter[9]+=bitcount[i+71]*pow(2,0);

     letter[10]+=bitcount[i+72]*pow(2,3);
      letter[10]+=bitcount[i+73]*pow(2,2);
      letter[10]+=bitcount[i+74]*pow(2,1);
      letter[10]+=bitcount[i+75]*pow(2,0);
      letter[11]+=bitcount[i+76]*pow(2,3);
      letter[11]+=bitcount[i+77]*pow(2,2);
      letter[11]+=bitcount[i+78]*pow(2,1);
      letter[11]+=bitcount[i+79]*pow(2,0);
/*
      sprintf(buf," result : %x %x %x %x %x %x %x %x %x %x %x %x !\n\r\0",letter[0],letter[1],letter[2],letter[3],letter[4],letter[5],letter[6],letter[7],letter[8],letter[9],letter[10],letter[11]);
      buf_len = strlen(buf);
      sci_display(sciREG1, (uint8 *)buf, buf_len);
*/
    switch (letter[0]){
    case 0xd7 :

        switch (letter[1])
        {
            case 0xd7 :

                if(letter[5]==0xd5) remote_now=7;
                else                remote_now=3;
                if(letter[2]==0xd5) remote_now=6;
            break;

            case 0xd5 :

                if(letter[2]==0xdd)
                {
                    switch (letter[3])
                    {
                        case 0xdd : remote_now='h';   // sharp
                        break;
                        case 0xd5 : remote_now=1;
                        break;
                        case 0xd7 : remote_now=8;
                        break;
                    }
                }
                else
                {
                    switch (letter[3])
                    {
                        case 0xd5 : remote_now=9;
                        break;
                        case 0xd7 : remote_now=0;
                        break;
                    }
                }
            break;
        }
    break;


    case 0xd5 :
        switch (letter[1])
        {
            case 0xdd :
                switch(letter[2])
                {   case 0xdd :
                        if(letter[3]==0xd7)
                                    remote_now='s';   //s=star;
                        else        remote_now=2;
                    break;
                    case 0xd7 :     remote_now='r';   //r=right
                    break;
                    case 0xd5 :     remote_now='d';   //d=down
                    break;
                }
            break;

            case 0xd7 :
                if(letter[2]==0xd7) remote_now='k';   //o=ok
                else                remote_now=4;
            break;

            case 0xd5 :
                switch (letter[3])
                {
                    case 0xdd :     remote_now='u';   //u= up
                    break;
                    case 0xd5 :     remote_now='l';   //l= left
                    break;
                    case 0xd7 :     remote_now=5;
                    break;
                }
            break;
        }
    }


/*
    fal=0;
      for(i=0;i<12;i++)
      {
          if(letter[i]!=letterok[i])
              fal++;
      }

      if(fal<=1){
          gioSetPort(gioPORTB, gioGetPort(gioPORTB)^ 0x0000040);
          remote_now++;
      }
*/
      i=0;
      for(i=0;i<12;i++)
          letter[i]=0;

}



// het interrrupt

#if 0
void pwmEnableNotification(hetBASE_t * hetREG, uint32 pwm, uint32 notification)
{
    hetREG->FLG     = notification << (pwm << 1U);
    hetREG->INTENAS = notification << (pwm << 1U);
}



/** @fn void pwmDisableNotification(hetBASE_t * hetREG, uint32 pwm, uint32 notification)
*   @brief Enable pwm notification
*   @param[in] hetREG Pointer to HET Module:
*              - hetREG1: HET1 Module pointer
*              - hetREG2: HET2 Module pointer
*   @param[in] pwm Pwm signal:
*              - pwm0: Pwm 0
*              - pwm1: Pwm 1
*              - pwm2: Pwm 2
*              - pwm3: Pwm 3
*              - pwm4: Pwm 4
*              - pwm5: Pwm 5
*              - pwm6: Pwm 6
*              - pwm7: Pwm 7
*   @param[in] notification Pwm notification:
*              - pwmEND_OF_DUTY:   Notification on end of duty
*              - pwmEND_OF_PERIOD: Notification on end of end period
*              - pwmEND_OF_BOTH:   Notification on end of both duty and period
*/
/* SourceId : HET_SourceId_008 */
/* DesignId : HET_DesignId_008 */
/* Requirements : HL_CONQ_HET_SR17 */
#endif


/* USER CODE END */
