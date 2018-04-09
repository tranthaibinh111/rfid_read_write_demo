/*
 * 
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 * 
 * PCD (Proximity Coupling Device): NXP MFRC522 Contactless Reader IC
 * PICC (Proximity Integrated Circuit Card): A card or tag using the ISO 14443A interface, eg Mifare or NTAG203.
 */
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN			    9				// Cong RST/Reset
#define SS_PIN			    10			// Cong SPI SS
#define MF_KEY_SIZE		  6				// Chuan Crypto1 cua NXP Semiconductors quy dinh 48bit = 6byte
#define MF_BLOCK_SIZE 	16			// 16 byte PICC
#define CRC_A			      2				// 2 bytes cyclic redundancy check A

MFRC522 mfrc522(SS_PIN, RST_PIN);	// Khoi tao MFRC522

//*****************************************************************************************//
void setup() {
	Serial.begin(9600);		// Cai dat cong ket noi voi PC
	SPI.begin();			    // Cai dat SPI
	mfrc522.PCD_Init();		// Cai dat dau doc the
	Serial.println(F("Doc thong tin ca nhan cua the:"));
}
//*****************************************************************************************//

//*****************************************************************************************//
void loop() {
	// Chuan bi khoa
	MFRC522::MIFARE_Key key;
	for(byte i = 0; i < MF_KEY_SIZE; i++) key.keyByte[i] = 0xFF;
	
	byte block = 1; 	// MIFARE Classic 1K (MF1S503x) 16 sectors * 4 blocks
	byte len   = MF_BLOCK_SIZE + CRC_A;
  byte buffer1[len];
	MFRC522::StatusCode status;

	//-------------------------------------------

	// Kiem tra co phai la the moi khong
	if ( !mfrc522.PICC_IsNewCardPresent()) return;

	// Chon the doc
	if ( !mfrc522.PICC_ReadCardSerial()) return;

	//-------------------------------------------

	// The hien thong tin chi tiet cua the (UID, SAK, PICC)
	mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid));

	//-------------------------------------------
	Serial.print(F("Name: "));
			
	// Lay ten chu the
	status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
	if (status != MFRC522::STATUS_OK) {
		Serial.print(F("Xac thuc the that bai: "));
		Serial.println(mfrc522.GetStatusCodeName(status));
	}

	status = mfrc522.MIFARE_Read(block, buffer1, &len);
	if (status != MFRC522::STATUS_OK) {
		Serial.print(F("Doc the that bai: "));
		Serial.println(mfrc522.GetStatusCodeName(status));
	}

	// Get ten chu the
	for (uint8_t i = 0; i < MF_BLOCK_SIZE; i++){
		Serial.write(buffer1[i]);
	}

	//----------------------------------------
	Serial.println(F("\n**Ket thuc doc thong tin**\n"));

	delay(1000);

	mfrc522.PICC_HaltA();
	mfrc522.PCD_StopCrypto1();

}
//*****************************************************************************************//
