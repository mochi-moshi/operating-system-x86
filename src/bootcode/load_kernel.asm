BITS 16
section .text
global second_stage, vbe_info_block, mode_info_block, SMAP_entries
extern DK_transfer_buffer_es, DK_transfer_buffer_bx, find_entry, END_OF_BOOTCODE, print, print_ax, BS_NumSecPerFAT, BS_BytesPerSec, BS_NumRootEnt, print_hex, print_dec
kernel_name: db "kernel_lbin",0
kernel_msg: db "Loading kernel into memory...", 0
kernel_success: db "Success", 0xA, 0xD, "Entering 32bit mode...", 0xA, 0xD, 0

memory_message: db "Loading SMAP entries...", 0xA, 0xD, 0
video_message: db "Loading VBE information...", 0xA, 0xD, 0
second_stage:
    xor ax, ax
    mov es, ax

    mov si, memory_message
    call print

    get_memory_map:
        mov di,  SMAP_entries+2
        xor ebx, ebx
        xor bp,  bp
        mov edx, 'PAMS'
        mov eax, 0xE820
        mov [es:di+20], dword 1
        mov ecx, 24
        int 0x15
        jc .error

        cmp eax, 'PAMS'
        jne .error
        test ebx, ebx
        jz  .error
        jmp .start
    .next_entry:
        mov edx, 'PAMS'
        mov ecx, 24
        mov eax, 0xE820
        int 0x15
    .start:
        jcxz .skip_entry
        mov ecx, [es:di+8]
        or  ecx, [es:di+12]
        jz .skip_entry
    .good_entry:
        inc bp
        add di, 24
    .skip_entry:
        test ebx, ebx
        jz  .done
        jmp .next_entry
    .error:
        stc
    .done:
        mov [SMAP_entries], bp
        clc
    
    mov si, video_message
    call print

    ; setup vbe info
    xor ax, ax
    mov es, ax
    mov ah, 0x4F
    mov di, vbe_info_block
    int 0x10

    cmp ax, 0x4F
    jne error

    mov ax, word [vbe_info_block.video_mode_pointer]
    mov [offset],    ax
    mov ax, word [vbe_info_block.video_mode_pointer+2]
    mov [t_segment], ax

    mov fs, ax
    mov si, [offset]

    .find_mode:             ; TODO: add resolution selector
        mov dx, [fs:si]
        add si, 2
        mov [offset], si
        mov [mode],   dx

        cmp dx, word 0xFFFF ; end of list
        je end_of_modes

        mov ax, 0x4F01
        mov cx, [mode]
        or cx, 0x4000
        mov di, mode_info_block
        int 0x10

        cmp ax, 0x4F
        jne error

        mov dx, [mode_info_block.x_resolution]
        shr dx, 4
        cmp dx, 0
        jl .not_resolution
        mov ax, [mode_info_block.y_resolution]
        mov cl, 9
        div cl
        cmp ah, 0
        jne .not_resolution

        mov dx, [mode_info_block.x_resolution]
        call print_dec
        mov ax, 0x0E00 + 'x'
        int 0x10

        mov dx, [mode_info_block.y_resolution]
        call print_dec
        mov ax, 0x0E00 + ' '
        int 0x10
        movsx dx, byte [mode_info_block.bits_per_pixel]
        call print_dec

        mov ax, 0x0E00 + 0x0A
        int 0x10
        mov ax, 0x0E00 + 0x0D
        int 0x10

        .not_resolution:
        mov ax, [width]
        cmp ax, [mode_info_block.x_resolution]
        jne .next_mode

        mov ax, [height]
        cmp ax, [mode_info_block.y_resolution]
        jne .next_mode

        mov ax, [bpp]
        cmp al, [mode_info_block.bits_per_pixel]
        jne .next_mode

        ;  cli
        ;  hlt

        ; ; enable mode
        ; mov ax, 0x4F02
        ; mov bx, [mode]
        ; ; or bx,  0x4000
        ; xor di, di
        ; int 0x10

        ; mov ax, 0x4F

        ; cmp ax, 0x4F
        ; jne error

        jmp Load_Kernel
    .next_mode:
        mov ax, [t_segment]
        mov fs, ax
        mov si, [offset]
        jmp .find_mode
    error:
        mov si, video_error
        call print
        cli
        hlt
    video_error: db "There was an error loading video mode", 0
    end_of_modes:
        mov si, veom
        call print
        cli
        hlt
    veom: db "Hit end of video modes", 0

Load_Kernel:
    mov si, kernel_msg
    call print

    mov word [DK_transfer_buffer_es], 0x1000
    mov word [DK_transfer_buffer_bx], 0
    mov si, kernel_name
    call find_entry

    mov si, kernel_success
    call print

    ; enable mode
    mov ax, 0x4F02
    mov bx, [mode]
    ; or bx,  0x4000
    xor di, di
    int 0x10

    mov ax, 0x4F

    cmp ax, 0x4F
    jne error

Load_GDT:
    cli
    lgdt [ptgdt]
    sti

    ; enable A20 through keyboard microcontroller
    mov al, 2
    out 0x92, al

    ; enter protected mode
    cli
    mov eax, cr0
    or eax, 1
    mov cr0, eax


    jmp 0x08:pre_kern

ptgdt:
    dw GDT.end - GDT.null - 1
    dd GDT
GDT:
    .null:
        dd 0
        dd 0
    .kernel_code:
        dw 0xFFFF        ; limit low
        dw 0            ; base low
        db 0            ; base mid
        db 10011010b    ; access
        db 11001111b    ; granuality
        db 0            ; base high
    .kernel_data:
        dw 0xFFFF        ; limit low
        dw 0            ; base low
        db 0            ; base mid
        db 10010010b    ; access
        db 11001111b    ; granuality
        db 0            ; base high
    ; .user_code:
    ;     dw 0xFFFF        ; limit low
    ;     dw 0            ; base low
    ;     db 0            ; base mid
    ;     db 11111010b    ; access
    ;     db 11001111b    ; granuality
    ;     db 0            ; base high
    ; .user_data:
    ;     dw 0xFFFF        ; limit low
    ;     dw 0            ; base low
    ;     db 0            ; base mid
    ;     db 11110010b    ; access
    ;     db 11001111b    ; granuality
    ;     db 0            ; base high
    ; .globals_gate:
    ;     dw (GLOBALS.OFFSET and 0xFFFF)        ; limit low
    ;     dw 0x8          ; code segment
    ;     db 0            ; base mid
    ;     db 11101100b    ; access
    ;     db 0            ; granuality
    ;     db (GLOBALS.OFFSET shr 16)            ; base high
    .end:

BITS 32

pre_kern:
    cli

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x7FFFF
    mov ebp, esp
    
    xor eax,eax
    xor ebx,ebx
    xor ecx,ecx
    xor edx,edx
    xor edi,edi
    xor esi,esi

    jmp 0x10000
width:      dw 1280 ;800
height:     dw 720 ;600
bpp:        db 32
offset:     dw 0
t_segment:  dw 0
mode:       dw 0
align(512)
vbe_info_block: ; 0x8400
    .vbe_signature: db 'VBE2'
    .vbe_version: dw 0x300
    .oem_string_pointer: dd 0
    .capabilities: dd 0
    .video_mode_pointer: dd 0
    .total_memory: dw 0
    .oem_software_rev: dw 0
    .oem_vendor_name_pointer: dd 0
    .oem_product_name_pointer: dd 0
    .oem_product_revision_pointer: dd 0
    .reserved: times 222 db 0
    .oem_data: times 256 db 0
align(512)
mode_info_block: ; 0x8600
    .mode_attributes: dw 0
    .window_a_attributes: db 0
    .window_b_attributes: db 0
    .window_granularity: dw 0
    .window_size: dw 0
    .window_a_segment: dw 0
    .window_b_segment: dw 0
    .window_function_pointer: dd 0
    .bytes_per_scanline: dw 0

    .x_resolution: dw 0
    .y_resolution: dw 0
    .x_charsize: db 0
    .y_charsize: db 0
    .number_of_planes: db 0
    .bits_per_pixel: db 0
    .number_of_banks: db 0
    .memory_model: db 0
    .bank_size: db 0
    .number_of_image_pages: db 0
    .reserved1: db 1

    .red_mask_size: db 0
    .red_field_position: db 0
    .green_mask_size: db 0
    .green_field_position: db 0
    .blue_mask_size: db 0
    .blue_field_position: db 0
    .reserved_mask_size: db 0
    .reserved_field_position: db 0
    .direct_color_mode_info: db 0

    .physical_base_pointer: dd 0
    .reserved2: dd 0
    .reserved3: dw 0

    .linear_bytes_per_scan_line: dw 0

    .bank_number_of_image_pages: dw 0
    .linear_number_of_image_pages: db 0
    .linear_red_mask_size: db 0
    .linear_red_field_position: db 0
    .linear_green_mask_size: db 0
    .linear_green_field_position: db 0
    .linear_blue_mask_size: db 0
    .linear_blue_field_position: db 0
    .linear_reserved_mask_size: db 0
    .linear_reserved_field_position: db 0
    .max_pixel_clock: dd 0
    .reserved4: times 190 db 0
align(512)
SMAP_entries:
    .number_of_entries: dw 0
    times 510 db 0