#include <iostream>
#include <fstream>
#include <windows.h>
class ReaderBMP {
private:

	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER infoHeader;
	uint8_t* pixelData;
	int width, height, bitCount;

public:

	ReaderBMP() : fileHeader{}, infoHeader{}, pixelData(nullptr), width(0), height(0), bitCount(0) {}

	bool openBMP(const std::string& fileName) {
		std::ifstream file(fileName, std::ios::binary);
		if (!file.is_open()){
			std::cerr << "Не удалось открыть файл: " << fileName << std::endl;
			return false;
		}

		file.read(reinterpret_cast<char*> (&fileHeader), sizeof(BITMAPFILEHEADER));
		file.read(reinterpret_cast<char*>(&infoHeader), sizeof(BITMAPINFOHEADER));

		if (fileHeader.bfType != 0x4D42 || (infoHeader.biBitCount != 24 && infoHeader.biBitCount != 32)) {
			std::cerr << "Неверный формат файла. Поддерживается только BMP 24 или 32 бита." << std::endl;
			file.close();
			return false;
		}

		width = infoHeader.biWidth;
		height = infoHeader.biHeight;
		bitCount = infoHeader.biBitCount;


		file.seekg(fileHeader.bfOffBits, std::ios::beg);


		int rowSize = ((bitCount * width + 31) / 32) * 4;
		pixelData = new uint8_t[rowSize * height];

		file.read(reinterpret_cast<char*>(pixelData), rowSize * height);
		if (!file) {
			std::cerr << "Ошибка при чтении данных изображения." << std::endl;
			delete[] pixelData;
			pixelData = nullptr;
			file.close();
			return false;
		}

		file.close();
		return true;
	}

	void displayBMP() const {
		if (!pixelData) {
			std::cerr << "Изображение не загружено." << std::endl;
			return;
		}
		int rowSize = ((bitCount * width + 31) / 32) * 4;

		for (int y = height - 1; y >= 0; --y) {  
			for (int x = 0; x < width; ++x) {
				uint8_t* pixel = &pixelData[y * rowSize + x * (bitCount / 8)];

				uint8_t blue = pixel[0];
				uint8_t green = pixel[1];
				uint8_t red = pixel[2];
				if (red == 255 && green == 255 && blue == 255) {
					std::cout << " ";
				}
				else if (red == 0 && green == 0 && blue == 0) {
					std::cout << "#";
				}
				else if (red == 64 && green == 64 && blue == 64) { //Для серого цвета
					std::cout << "#";
				}
			}
			std::cout << std::endl;
		}
	}

	void closeBMP() {
		if (pixelData) {
			delete[] pixelData;
			pixelData = nullptr;
		}
	}

	~ReaderBMP() {
		closeBMP();  
	}
};

//Передавать в аргументы командной строки абсолютный путь к файлу
int main(int argc, char* argv[]) {
	setlocale(LC_ALL, "Ru");

	SetConsoleOutputCP(CP_UTF8);

	if (argc != 2) {
		std::cerr << "Необходимый ввод: drawBmp.exe <путь к BMP файлу> " << std::endl;
		return 1;
	}

	std::string fileName = argv[1];

	ReaderBMP readBmp;

	if (readBmp.openBMP(fileName)) {
		readBmp.displayBMP();
		readBmp.closeBMP();
	}

	return 0;
}