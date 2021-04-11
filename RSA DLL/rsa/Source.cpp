#include <iostream>
#include <vector>
#include <iomanip>
#include <cmath>
#include <climits>
#include <stack>
#include <random>
#include <fstream>

using namespace std;

//Структура ключа
typedef struct {
	uint64_t e;
	uint64_t m;
} key;

//Расширенный алгоритм Евклида
int64_t gcdex(int64_t a, int64_t b, int64_t &x, int64_t &y) {
	if (a == 0) {
		x = 0;
		y = 1;
		return b;
	}
	int64_t x1, y1;
	int64_t d = gcdex(b % a, a, x1, y1);
	x = y1 - (b / a) * x1;
	y = x1;
	return d;
}

//Обратное по модулю
int64_t invmod(int64_t a, int64_t m) {
	int64_t x, y;
	gcdex(a, m, x, y);
	x = (x % m + m) % m;
	return x;
}

uint64_t sqr(uint64_t x) {
	return x * x;
}

//Быстрое возведение в степень
uint64_t binpow(uint64_t a, uint64_t e, uint64_t mod = LLONG_MAX) {
	return e == 0 ? 1 : (e & 1U ? a * binpow(a, e - 1, mod) % mod : sqr(binpow(a, e / 2, mod)) % mod);
}

//Генерация пары ключей
pair<key, key> gen_keys(uint64_t p, uint64_t q) {
	uint64_t phi = (p - 1) * (q - 1);
	uint64_t n = p * q;
	//Простое число Мерсенна, обычно используется в RSA в виде открытой экспоненты для увеличения производительности
	uint64_t e = 65537;
	uint64_t d = invmod(e, phi);
	return{ { e, n },
	{ d, n } };
}

//Размер блока шифрования ключа
uint8_t get_chunk_size(key k) {
	return 32 - __builtin_clz(k.m);
}

//Резбиение/обьединение данных в блоки
vector<uint64_t> resize(const vector<uint64_t> &data, uint8_t in_size, uint8_t out_size) {
	vector<uint64_t> res;
	uint8_t done = 0;
	uint64_t cur = 0;
	for (uint64_t byte : data)
		for (uint8_t i = 0; i < in_size; i++) {
			cur = (cur << 1U) + (((uint64_t)byte & (1U << (uint64_t)(in_size - 1 - i))) != 0);
			done++;
			if (done == out_size) {
				done = 0;
				res.push_back(cur);
				cur = 0;
			}
		}
	//Дополнение нулями
	if (done != 0)
		res.push_back(cur << (uint64_t)(out_size - done));
	return res;
}

//(Де)шифрование
vector<uint8_t> process_bytes(const vector<uint8_t> &data, key k, bool encrypt) {
	vector<uint64_t> data_64(data.size());
	for (int i = 0; i < data.size(); i++)
		data_64[i] = (uint64_t)data[i];
	vector<uint64_t> resized_data = resize(data_64, 8, get_chunk_size(k) - encrypt); //Если мы шифруем, то размер блока K - 1, иначе K
	vector<uint64_t> encrypted_data(resized_data.size());
	for (int i = 0; i < resized_data.size(); i++)
		encrypted_data[i] = binpow(resized_data[i], k.e, k.m);
	vector<uint64_t> result_64 = resize(encrypted_data, get_chunk_size(k) - !encrypt, 8);
	vector<uint8_t> result(result_64.size());
	for (int i = 0; i < result_64.size(); i++)
		result[i] = (uint8_t)result_64[i];
	return result;
}

//Функции работы с файлами
vector<uint8_t> read_bytes(const char *filename) {
	ifstream fin(filename);
	fin.seekg(0, ios::end);
	size_t len = fin.tellg();
	auto *bytes = new char[len];
	fin.seekg(0, ios::beg);
	fin.read(bytes, len);
	fin.close();
	vector<uint8_t> ret(len);
	for (int i = 0; i < len; i++)
		ret[i] = (uint8_t)bytes[i];
	delete[] bytes;
	return ret;
}

void write_bytes(const char *filename, const vector<uint8_t> &data) {
	ofstream fout(filename);
	char *buf = new char[data.size()];
	for (int i = 0; i < data.size(); i++)
		buf[i] = (char)data[i];
	fout.write(buf, data.size());
	fout.close();
}

void help() {
	cout << "---- tinyRSA\n"
		"e <exp> <mod> <file in> <file out> -- Encrypt file\n"
		"d <exp> <mod> <file in> <file out> -- Decrypt file\n"
		"g -- Generate key pair from default primes\n"
		"G <p> <q> -- Generate key pair from primes\n"
		"h -- Help\n"
		"q -- Exit\n" << endl;
}

int main() {
	help();
	while (true) {
		char cmd;
		cin >> cmd;
		if (cmd == 'q') {
			cout << "Bye." << endl;
			break;
		}
		else if (cmd == 'e') {
			uint64_t e, m;
			string fin, fout;
			cin >> hex >> e >> hex >> m >> fin >> fout;
			auto in = read_bytes(fin.c_str());
			write_bytes(fout.c_str(), process_bytes(in, { e, m }, true));
			cout << "Done." << endl;
		}
		else if (cmd == 'd') {
			uint64_t e, m;
			string fin, fout;
			cin >> hex >> e >> hex >> m >> fin >> fout;
			auto in = read_bytes(fin.c_str());
			write_bytes(fout.c_str(), process_bytes(in, { e, m }, false));
			cout << "Done." << endl;
		}
		else if (cmd == 'g') {
			auto key_pair = gen_keys(3557, 2579);
			cout << "OPEN KEY  : " << hex << key_pair.first.e << ' ' << hex << key_pair.first.m << endl;
			cout << "SECRET KEY: " << hex << key_pair.second.e << ' ' << hex << key_pair.second.m << endl;
		}
		else if (cmd == 'G') {
			uint64_t p, q;
			cin >> p >> q;
			auto key_pair = gen_keys(p, q);
			cout << "OPEN KEY  : " << hex << key_pair.first.e << ' ' << hex << key_pair.first.m << endl;
			cout << "SECRET KEY: " << hex << key_pair.second.e << ' ' << hex << key_pair.second.m << endl;
		}
		else if (cmd == 'h')
			help();
	}
	return 0;
}