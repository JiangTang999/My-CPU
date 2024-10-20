#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "led.h"
#include "usart.h"
#include "sys.h"
#include "mpu6050.h"  
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h" 
#include "OLED.h"
#include "button.h"

int main(void)
{
    float pitch, roll, yaw;
    short vx, vy, vz, ax, ay, az;  // 定义用于存储加速度和角速度的变量
 char pitch_str[20], roll_str[20], yaw_str[20];  // 定义字符数组存储转换后的字符串
    char ax_str[20], ay_str[20], az_str[20];  // 存储加速度的字符串
    char vx_str[20], vy_str[20], vz_str[20];  // 存储角速度的字符串
	
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	
    uart_init(115200);
    delay_init();
    LED_Init();	
    MPU_Init();		
    OLED_Init();
	button_Init();
    
    while (mpu_dmp_init()) {
        delay_ms(20);
    }
	
    while (1) {

        if (mpu_dmp_get_data(&pitch, &roll, &yaw) == 0) {
          //LED = ~LED;
        }

		if (button_Scan()== 0) {
          LED = ~LED;
        }
//        // 获取角速度和加速度数据
//      
      sprintf(pitch_str, " %.2f", pitch);
      sprintf(roll_str, " %.2f", roll);
      sprintf(yaw_str, " %.2f", yaw);

//        // 将加速度数据转换为字符串
//		sprintf(vx_str, "Gx: %d", vx);
//        sprintf(vy_str, "Gy: %d", vy);
//        sprintf(vz_str, "Gz: %d", vz);
//		
//        sprintf(ax_str, "Ax: %d", ax);
//        sprintf(ay_str, "Ay: %d", ay);
//        sprintf(az_str, "Az: %d", az);
//		OLED_ShowString(1, 1, vx_str);
//        OLED_ShowString(2, 1, vy_str);
//        OLED_ShowString(3, 1, vz_str);
        // 显示加速度值
//        OLED_ShowString(1, 1, ax_str);
//        OLED_ShowString(2, 1, ay_str);
//        OLED_ShowString(3, 1, az_str);


        // OLED 显示角度和加速度值
        OLED_ShowString(1, 1, pitch_str);
        OLED_ShowString(2, 1, roll_str);
        OLED_ShowString(3, 1, yaw_str);

//    uint8_t MyArray[] = {pitch, roll, yaw};	//定义数组
//	Serial_SendArray(MyArray, 3);		//串口发送一个数组
	char String[100]; 
	float myFloat = pitch;             // 定义浮点数
	
	//sprintf(String, "Float Value: %.2f\r\n", myFloat);  // 使用sprintf格式化小数，保留两位小数
	if (button_Scan()==0)
	{
	for(int i=1;i<151;i++)
		{
		mpu_dmp_get_data(&pitch, &roll, &yaw);
		sprintf(pitch_str, " %.2f", pitch);
        sprintf(roll_str, " %.2f", roll);
        sprintf(yaw_str, " %.2f", yaw);
		OLED_ShowString(1, 1, pitch_str);
        OLED_ShowString(2, 1, roll_str);
        OLED_ShowString(3, 1, yaw_str);
		Serial_SendString(pitch_str);      
		Serial_SendString(roll_str);      
		Serial_SendString(yaw_str);   
		Serial_SendString("\r\n");
		delay_ms(10);
		}
		delay_ms(50000);
		delay_ms(50000);
		delay_ms(50000);
	}
	else
	{}
		
    } 	
}
