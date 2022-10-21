//   Copyright 2022 Will Thomas
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.

#define _GNU_SOURCE
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <sched.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

uint64_t str_int_dec(int8_t* a) {
	uint64_t b = 0;
	for(uint8_t i = 0; i < 20; i++) {
		if (a[i] == 0 || a[i] == ')') {
			return b;
		}
		b *= 10;
		if (a[i] == '1') {
			b += 1;
		}
		else if (a[i] == '2') {
			b += 2;
		}
		else if (a[i] == '3') {
			b += 3;
		}
		else if (a[i] == '4') {
			b += 4;
		}
		else if (a[i] == '5') {
			b += 5;
		}
		else if (a[i] == '6') {
			b += 6;
		}
		else if (a[i] == '7') {
			b += 7;
		}
		else if (a[i] == '8') {
			b += 8;
		}
		else if (a[i] == '9') {
			b += 9;
		}
		else if (a[i] != '0' && a[i] != ')') {
			return -1;
		}
	}
}

int32_t alc_shm(uint64_t sz) {
	int8_t n[8];
	n[0] = '/';
	n[7] = 0;
	for (uint8_t i = 1; i < 6; i++) {
		n[i] = (rand() & 23) + 97;
	}

	int32_t fd = shm_open(n, O_RDWR | O_CREAT | O_EXCL, S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH);
	shm_unlink(n);
	ftruncate(fd, sz);

	return fd;
}

int8_t main(int32_t argc, int8_t** argv) {
	if (argc != 3) {
		printf("usage: execbin [binary.bin] [size of stack(bytes)]\n");
		return -1;
	}
	
	int32_t fd = open(argv[1], O_RDONLY);
	if (fd == -1) {
		printf("failed to open file '%s'\n", argv[1]);
		return -1;
	}
	struct stat fs;
	fstat(fd, &fs);
	uint8_t* bin = mmap(0, fs.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	close (fd);
	
	uint64_t sz = fs.st_size + str_int_dec(argv[2]);
	fd = alc_shm(sz);
	uint8_t* mem = mmap(0, sz, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, fd, 0);
	close(fd);
	memcpy(mem, bin, fs.st_size);
	munmap(bin, fs.st_size);
	
	int32_t pid = clone(mem, mem + sz, CLONE_VFORK, 0);	
	printf("%i\n", pid);
	
	munmap(mem, sz);
	return 0;
}

