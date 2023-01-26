BITS 16
    section .text

extern BS_NumSecPerFAT, BS_BytesPerSec, DK_number_of_sectors, DK_lower_lba, DK_upper_lba, load_lba_sector, END_OF_BOOTCODE, print, print_ax, DK_transfer_buffer_es
global find_entry, print_dec
find_entry:
    push es
    push ax
    push bx
    push dx
    push cx
    mov ax, word [BS_NumSecPerFAT]
    mov bx, word [BS_BytesPerSec]
    mul bx
    mov bx, END_OF_BOOTCODE
    shr bx, 4
    mov es, bx
    mov di, ax
search:
    push si
    mov cx, 11
equal:
    repe cmpsb
    je found
    pop bx
    sub si, bx
    sub di, si
    add di, 32
    mov si, bx
    jmp search
found:
    add di, 0x1a-11 ; offset to starting cluster
more:
    mov si, word [es:di]
    mov di, si

    mov byte [DK_number_of_sectors], 1
    mov word [DK_lower_lba], si
    mov word [DK_upper_lba], 0
    call load_lba_sector

    shl di, 1
    mov ax, word [es:di]
    and ax, 0xFFF0
    cmp ax, 0xFFF0
    je done
    add word [DK_transfer_buffer_es], 0x20
    jmp more
done:

    pop si
    pop cx
    pop dx
    pop bx
    pop ax
    pop es
    ret

print_di:
    push ax
    push ds
    push si
    mov si, es
    mov ds, si
    mov si, di
    mov ah, 0x0e
    .loop:
        lodsb
        cmp al, 0
        je .end
        int 0x10
        jmp .loop
    .end:
    pop si
    pop ds
    pop ax
    ret

print_dec:
    push ax
    push cx
    push bx
    push dx
    mov bl, 0
    mov ax, dx
    mov cx, 10000
    .loop:
        mov dx, 0
        div cx
        push dx
        mov ah, 0x0E
        cmp bl, 0
        jne .sig_found
        cmp al, 0
        je .no_sig
        mov bl, 1
        .sig_found:
        add al, '0'
        int 0x10
        .no_sig:
        mov dx, 0
        mov ax, cx
        mov cx, 10
        div cx
        mov cx, ax
        pop ax
        cmp cx, 0
        jne .loop
    pop dx
    pop bx
    pop cx
    pop ax
    ret