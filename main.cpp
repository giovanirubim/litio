#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <stdint.h>

#define BLOB_SIZE (1 << 12)

uint8_t blob[BLOB_SIZE];
uint8_t buffer[BLOB_SIZE];

void putbin(void* mem, int n) {
	uint8_t* p = (uint8_t*) mem;
	while (n--) {
		printf("%02x", *p++);
		putchar(n?' ':'\n');
	}
}

void xorBuffer() {
	uint32_t* src = (uint32_t*) blob;
	uint32_t* dst = (uint32_t*) buffer;
	int n = BLOB_SIZE/4;
	while (n--) {
		*dst ^= *src;
		++dst;
		++src;
	}
}

#ifdef __linux__
	void clear() {
		system("clear");
	}
#elif _WIN32
	void clear() {
		system("cls");
	}
#else
	void clear() {}
#endif

void pause() {
	printf("Pressione enter para continuar\n");
	while (getchar()!='\n');
}

void initBlob(char const pwd[]) {
	uint8_t last = 0;
	int pwdlen = strlen(pwd);
	for (int i=0; i<BLOB_SIZE; ++i) {
		last = blob[i] = (last * 31) + pwd[i % pwdlen] + i;
	}
}

void showPercentage(int index, int n) {
	double x = index;
	printf("%.2lf%%\n", x/n*100);
}

void encrypt(char const fname[]) {
	FILE* file = fopen(fname, "r+b");
	if (!file) {
		printf("Falha ao criptografar '%s'\n", fname);
		return;
	}
	printf("Encriptando %s\n", fname);
	fseek(file, 0L, SEEK_END);
	uint64_t size = ftell(file);
	fseek(file, 0L, SEEK_SET);
	uint64_t left = size;
	uint64_t index = 0;
	uint64_t count = 0;
	while (left) {
		uint32_t n = left < BLOB_SIZE ? left : BLOB_SIZE;
		fread(buffer, n, 1, file);
		xorBuffer();
		fseek(file, index, SEEK_SET);
		fwrite(buffer, n, 1, file);
		index += n;
		left -= n;
		if ((++count)%10000 == 0) {
			showPercentage(index, size);
		}
	}
	fclose(file);
}

int main(int argc, char const *argv[]) {
	if (argc == 1) {
		clear();
		puts("Nenhum arquivo");
		pause();
		clear();
		return 0;
	}
	char pwd1[256];
	char pwd2[256];
	for (;;) {
		clear();
		printf("Senha: ");
		scanf("%[^\n]%*c", pwd1);
		clear();
		printf("Repita a senha: ");
		scanf("%[^\n]%*c", pwd2);
		clear();
		if (strcmp(pwd1, pwd2)) {
			puts("As senhas nao conferem");
			pause();
		} else {
			clear();
			break;
		}
	}
	initBlob(pwd1);
	for (int i=1; i<argc; ++i) {
		encrypt(argv[i]);
	}
	puts("Terminado");
	pause();
	clear();
}
