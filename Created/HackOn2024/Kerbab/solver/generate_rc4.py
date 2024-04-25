from arc4 import ARC4

found = False

i = 0
while not found:
    key = i.to_bytes((len(hex(i))-1)//2,'little')
    if b'\x00' not in key:
      arc4 = ARC4(key)
      cipher = arc4.encrypt(b'A'*64 + b'\x00')
      print(f"{key = }")
      print(f"{cipher = }")
      found = (cipher[64] == 0x40)
      if found:
        if len(key) > 0:
          print(f"[+] KEY : {key} [+]")
        else:
          found = False
    i+=1
