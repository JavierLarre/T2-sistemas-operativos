#include "Osrms_File.h"



char *MemoryPath = NULL;
FILE *file = NULL;


static bool compare_unsigned_string(unsigned char *s1, char *s2, size_t n) {
    return !strncmp((char*) s1, s2, n);
}


char* get_memory_path() {
    return MemoryPath;
}

void set_memory_path(const char *path) {
    if (MemoryPath != NULL) {
        free(MemoryPath);
    }
    MemoryPath = calloc(strlen(path) + 1, sizeof(char));
    strcpy(MemoryPath, path);
    
    if (file != NULL) {
        fclose(file);
    }
    file = fopen(MemoryPath, "rb+");
}
FILE *get_memory_file() {
    return file;
}

void close_memory() {
    if (MemoryPath != NULL) {
        free(MemoryPath);
    }
    if (file != NULL) {
        fclose(file);
    }
}


Process *buscar_proceso(int pid) {
    Process *p = calloc(1, sizeof(Process));
    const size_t tamano_proceso = sizeof(Process);
    const size_t tamano_en_memoria = 256;
    unsigned int base_address = 0;
    for (int i = 0; i < N_PROCESS; i++) {
        fseek(file, base_address, SEEK_SET);
        fread(p, tamano_proceso, 1, file);
        p->address_on_memory = base_address;
        if (p->valid && p->pid == pid) return p;
        base_address += tamano_en_memoria;
    }
    free(p);
    return NULL;
}


bool file_is_valid(osrmsFile *f) {
    return f->valid;
}


bool file_name_is_name(osrmsFile *f, char *name) {
    return f->valid && compare_unsigned_string(f->name, name, sizeof(f->name));
}


osrmsFile *buscar_archivo(Process *p, char *name) {
    osrmsFile *osrms_file = calloc(1, sizeof(osrmsFile));
    const size_t tamano_archivo = 23;
    uint16_t base_address = p->address_on_memory + 13;
    fseek(file, base_address, SEEK_SET);
    for (int i = 0; i < N_FILE; i++) {
        fseek(file, i * tamano_archivo, SEEK_CUR);
        fread(osrms_file, tamano_archivo, 1, file);
        if (file_name_is_name(osrms_file, name)) {
            return osrms_file;
        }
    }
    free(osrms_file);
    return NULL;
}


Process **get_processes() {
    Process **processes = calloc(N_PROCESS, sizeof(Process*));
    const size_t tamano_proceso = 13;
    const size_t tamano_en_memoria = 256;
    unsigned int base_address = 0;
    for (int i = 0; i < N_PROCESS; i++) {
        processes[i] = calloc(1, sizeof(Process));
        fseek(file, base_address, SEEK_SET);
        fread(processes[i], tamano_proceso, 1, file);
        processes[i]->address_on_memory = base_address;
        base_address += tamano_en_memoria;
    }
    return processes;
}

void free_processes(Process **processes) {
    for (int i = 0; i < N_PROCESS; i++) {
        free(processes[i]);
    }
    free(processes);
}

osrmsFile **get_files(Process *p) {
    osrmsFile **files = calloc(N_FILE, sizeof(osrmsFile*));
    const size_t tamano_archivo = 23;
    uint16_t base_address = p->address_on_memory + 13;
    for (int i = 0; i < N_FILE; i++) {
        files[i] = calloc(1, sizeof(osrmsFile));
        fseek(file, base_address, SEEK_SET);
        fread(files[i], tamano_archivo, 1, file);
        base_address += tamano_archivo;
        // -1 to avoid the fseek in the next iteration
        // because the next iteration will do the fseek
        // in the correct position
        // copilot comentó esto
        // QUE FSEEK HACE QUE
    }
    return files;
}

void free_files(osrmsFile **files) {
    for (int i = 0; i < N_FILE; i++) {
        free(files[i]);
    }
    free(files);
}

uint8_t get_frame_bitmap_byte(int byte_index) {
    uint8_t byte;
    const uint32_t base_address = 8*KB + 128 + 128*KB;
    fseek(file, base_address + byte_index, SEEK_SET);
    fread(&byte, sizeof(uint8_t), 1, file);
    return byte;
}


uint8_t get_table_bitmap_byte(int byte_index) {
    uint8_t byte;
    const uint32_t base_address = 8*KB;
    fseek(file, base_address + byte_index, SEEK_SET);
    fread(&byte, sizeof(uint8_t), 1, file);
    return byte;
}


void save_process(Process *p) {
    fseek(file, p->address_on_memory, SEEK_SET);
    fwrite(p, 13, 1, file);
}
// Save data in PCB table.
void save_files(osrmsFile **files, Process *p) {
    const size_t tamano_archivo = sizeof(osrmsFile);
    uint16_t base_address = p->address_on_memory + 13;
    for (int i = 0; i < N_FILE; i++) {
        fseek(file, base_address, SEEK_SET);
        fwrite(files[i], tamano_archivo, 1, file);
        base_address += tamano_archivo - 1;
    }
}
// fo page table. Has 64 entries o 2 bytes each.
// Belongs to a process and starts at address_on_memory + 13 + 23*N_FILE
uint16_t *get_fo_page_table(Process *p) {
    uint16_t *fo_page_table = calloc(64, sizeof(uint16_t));
    uint16_t base_address = p->address_on_memory + 13 + 23*N_FILE;
    fseek(file, base_address, SEEK_SET);
    fread(fo_page_table, 2, 64, file); // reads a chunk of 128 bytes
    return fo_page_table;
}

// so page table. Has 64 entries o 2 bytes each. Total 128 bytes.
// Receives a number between 0 and 1023 that corresponds to the so page table in the so page table space.
// SO page table space starts at PCB + Page_Table_bitmap = 8KB + 128B
uint16_t *get_so_page_table(int so_page_table_number) {
    uint16_t *so_page_table = calloc(64, sizeof(uint16_t));
    uint16_t base_address = 8*KB + 128 + so_page_table_number * 128;
    fseek(file, base_address, SEEK_SET);
    fread(so_page_table, 2, 64, file); // reads a chunk of 128 bytes
    return so_page_table;
}

// gets VPN from a virtual address
// VAN : 5 bit 0b00000 + 12 bit VPN + 15 bit offset = 32 bit

uint16_t get_vpn(uint32_t virtual_address) {
    return virtual_address >> 15;
}

// split VPN in 2 parts of 6 bits each

uint8_t get_vpn1(uint16_t vpn) {
    return vpn >> 6;
}

uint8_t get_vpn2(uint16_t vpn) {
    return vpn & 0x3f;
}

// get physical address from a process and a file

uint32_t get_physical_address(Process *p, osrmsFile *f) {
    
    if (!p || !f) return 0;

    // dissasemble virtual address
    uint32_t offset = f->virtual_address && 0x7fff; // 15 bits menos significativos
    uint32_t vpn = get_vpn(f->virtual_address);
    uint8_t vpn1 = get_vpn1(vpn);
    uint8_t vpn2 = get_vpn2(vpn);

    // fo page table
    uint16_t *fo_page_table = get_fo_page_table(p);
    uint16_t sptn = fo_page_table[vpn1];

    // so page table

    uint16_t *so_page_table = get_so_page_table(sptn);
    uint16_t pfn = so_page_table[vpn2];

    // physical address

    uint32_t physical_adress = pfn | offset;

    free(fo_page_table);
    free(so_page_table);

    return physical_adress;
}

// get addresses occupied by a file in a process (max 5)
uint32_t *get_occupied_addres(Process *p) {

    uint32_t *addresses = calloc(N_FILE, sizeof(uint32_t));
    osrmsFile **files = get_files(p);
    for (int i = 0; i < N_FILE; i++) {
        addresses[i] = files[i]->virtual_address;
    }

    // sort addresses ascending

    for (int i = 0; i < N_FILE; i++) {
        for (int j = i + 1; j < N_FILE; j++) {
            if (addresses[i] > addresses[j]) {
                uint32_t temp = addresses[i];
                addresses[i] = addresses[j];
                addresses[j] = temp;
            }
        }
    }

    free_files(files);
    return addresses;

}

// get file sizes of a process (max 5)

uint32_t *get_file_sizes(Process *p) {
    uint32_t *sizes = calloc(N_FILE, sizeof(uint32_t));
    osrmsFile **files = get_files(p);
    for (int i = 0; i < N_FILE; i++) {
        sizes[i] = files[i]->size;
    }
    free_files(files);
    return sizes;
}

// search for space in the virtual memory to store a file
// returns the virtual address where the file will be stored

uint32_t get_virtual_address(Process *p, uint32_t file_size) {
    uint32_t *addresses = get_occupied_addres(p);
    uint32_t *sizes = get_file_sizes(p);
    uint32_t virtual_address = -1;
    
    for (int i = 0; i < N_FILE; i++) {
        // try to fit the file in the space between the files
        // if the file fits, return the virtual address

        // CASE 1: file fits starting from the beginning of the virtual memory
        if (i == 0 && addresses[i] >= file_size) {
            virtual_address = 0;
            break;
        }

        // CASE 2: file fits between two files

        if (i < N_FILE - 1 && addresses[i] + sizes[i] + file_size <= addresses[i + 1]) {
            virtual_address = addresses[i] + sizes[i];
            break;
        }

        // CASE 3: file fits at the end of the virtual memory

        if (i == N_FILE - 1 && addresses[i] + sizes[i] + file_size <= VIRTUAL_MEMORY_SIZE) {
            virtual_address = addresses[i] + sizes[i];
            break;
        }
    }

    free(addresses);
    free(sizes);
    return virtual_address;
}


Process *get_process_from_file(osrmsFile *f) {
    Process **processes = get_processes();
    for (int i = 0; i < N_PROCESS; i++) {
        osrmsFile **files = get_files(processes[i]);
        for (int j = 0; j < N_FILE; j++) {
            if (file_name_is_name(files[j], (char*) f->name)) {
                free_files(files);
                free_processes(processes);
                return processes[i];
            }
        }
        free_files(files);
    }
    free_processes(processes);
    return NULL;
}