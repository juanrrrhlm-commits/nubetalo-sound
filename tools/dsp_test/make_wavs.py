import wave, struct, math, random
sr=44100; secs=60
def write(name, gen):
    w=wave.open(name,'wb'); w.setnchannels(2); w.setsampwidth(2); w.setframerate(sr)
    buf=bytearray()
    for i in range(sr*secs):
        s=max(-1.0,min(1.0,gen(i))); v=int(s*32767)
        buf+=struct.pack('<hh',v,v)
    w.writeframes(bytes(buf)); w.close()
A=10**(-30/20)
write('tonos.wav', lambda i: A*(math.sin(2*math.pi*100*i/sr)+math.sin(2*math.pi*1000*i/sr)+math.sin(2*math.pi*8000*i/sr)))
random.seed(3); lp=[0.0]
def noise(i):
    lp[0]+=0.15*(random.gauss(0,1)-lp[0]); return lp[0]*(10**(-20/20))*3.2
write('musica.wav', noise)
print('wavs listos')
