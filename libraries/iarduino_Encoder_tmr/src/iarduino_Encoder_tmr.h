//	Библиотека для работы с энкодерами: http://iarduino.ru/shop/Expansion-payments/enkoder-trema-modul.html
//  Версия: 1.0.0
//  Последнюю версию библиотеки Вы можете скачать по ссылке: http://iarduino.ru/file/297.html
//  Подробное описание функции бибилиотеки доступно по ссылке: http://wiki.iarduino.ru/page/enkoder-trema-modul/
//  Библиотека является собственностью интернет магазина iarduino.ru и может свободно использоваться и распространяться!
//  При публикации устройств или скетчей с использованием данной библиотеки, как целиком, так и её частей,
//  в том числе и в некоммерческих целях, просим Вас опубликовать ссылку: http://iarduino.ru
//  Автор библиотеки: Панькин Павел sajaem@narod.ru
//  Если у Вас возникли технические вопросы, напишите нам: shop@iarduino.ru

#ifndef iarduino_Encoder_tmr_h
#define iarduino_Encoder_tmr_h

#if defined(ARDUINO) && (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#define	encLEFT			 -1													//	Константа указывающая, что цвет применяется ко всем светодиодам
#define	encRIGHT		  1													//	Константа указывающая, что цвет применяется ко всем светодиодам
	
class iarduino_Encoder_tmr{
	public:						iarduino_Encoder_tmr	(uint8_t, uint8_t);	//	Создание экземпляра класса			(№ вывода, № вывода)
					bool		begin					(void);				//	Инициализация работы с энкодером	(без параметров)
					int8_t		read					(void);				//	Чтение состояния энкодера			(без параметров)
	private:
		volatile	uint8_t		enc_NUM					=	0;				//	Определяем переменную для хранения номера подключённого энкодера
					void		func_TIMER2_SETREG		(uint32_t);			//	Функция конфигурирования таймера
};

class iarduino_Encoder_tmr_VolatileClass{
	public:
		volatile	uint8_t		enc_SUM					=	0;				//	Определяем переменную для хранения количества подключённых энкодеров
		volatile	uint8_t		pin_NUMa[8];								//	Объявляем массив с номерами выводов к которым подключены энкодеры
		volatile	uint8_t		pin_NUMb[8];								//	Объявляем массив с номерами выводов к которым подключены энкодеры
		volatile	uint8_t	*	pin_PRTa[8];								//	Объявляем массив с указателями на адреса входных портов выводов
		volatile	uint8_t	*	pin_PRTb[8];								//	Объявляем массив с указателями на адреса входных портов выводов
		volatile	uint8_t		pin_MSKa[8];								//	Объявляем массив с масками выводов для входных портов выводов
		volatile	uint8_t		pin_MSKb[8];								//	Объявляем массив с масками выводов для входных портов выводов
		volatile	uint8_t		val_BITS[8];								//	Объявляем массив с последними значениями прочитанными с выводов энкодера (abababab)
		volatile	int8_t		i,j;										//	Объявляем переменную для внутренних нужд
};

#endif