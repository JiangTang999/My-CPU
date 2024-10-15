#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "led.h"
#include "usart.h"
#include "sys.h"
#include "mpu6050.h"  
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h" 
#include "OLED.h"
#include "Serial.h"

int main(void)
{
    float position[2*100][3];
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
    
    while (mpu_dmp_init()) {
        delay_ms(20);
    }
	
    while (1) {
        LED = ~LED;  // 切换 LED 状态

        if (mpu_dmp_get_data(&pitch, &roll, &yaw) == 0) {
            LED = ~LED;
        }

       // 获取角速度和加速度数据

    // 调用函数采集数据
    if (get_pitch_roll_yaw(position) == 0) {
        // 打印采样数据
        for (int i = 0; i < 2 * 100; i++) {
            printf("Data %d: [Pitch: %.2f, Roll: %.2f, Yaw: %.2f]\n", i+1, position[i][0], position[i][1], position[i][2]);
        }
    } else {
        printf("数据采集失败。\n");
    }
		
      sprintf(pitch_str, "Pitch: %.2f", pitch);
      sprintf(roll_str, "Roll: %.2f", roll);
      sprintf(yaw_str, "Yaw: %.2f", yaw);

//        // 将加速度数据转换为字符串
//				sprintf(vx_str, "Gx: %d", vx);
//        sprintf(vy_str, "Gy: %d", vy);
//        sprintf(vz_str, "Gz: %d", vz);
//		
//        sprintf(ax_str, "Ax: %d", ax);
//        sprintf(ay_str, "Ay: %d", ay);
//        sprintf(az_str, "Az: %d", az);

        // OLED 显示角度和加速度值
        OLED_ShowString(1, 1, pitch_str);
        OLED_ShowString(2, 1, roll_str);
        OLED_ShowString(3, 1, yaw_str);

//    uint8_t MyArray[] = {pitch, roll, yaw};	//定义数组
//	Serial_SendArray(MyArray, 3);		//串口发送一个数组
				Serial_SendString(pitch_str);      
				Serial_SendString(roll_str);          		
				Serial_SendString(yaw_str);          		
//			OLED_ShowString(1, 1, vx_str);
//      OLED_ShowString(2, 1, vy_str);
//      OLED_ShowString(3, 1, vz_str);
        // 显示加速度值
//        OLED_ShowString(1, 1, ax_str);
//        OLED_ShowString(2, 1, ay_str);
//        OLED_ShowString(3, 1, az_str);


    } 	
}
