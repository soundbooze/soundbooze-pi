
## Alacarte Stack

#### Direct native Jack client

- Pure console, 0-gui 

#### (Guitarix 0.37.3)  

[faust] ported from c++ to c

![alt text](https://raw.githubusercontent.com/soundbooze/soundbooze-pi/master/jack/tube.png "Home")

#### Compile

```
$ cat *.c | grep gcc > compile.sh && chmod 755 compile.sh
$ ./compile.sh 
$ rm -f compile.sh
```

#### JACKD

- Frames / Period : 256
- Periods / Buffer : 3

#### TODO (generative)

- tube style transfer
- impulse style transfer
- smooth transition
- parallel
- characteristic-learning {parameter || port-connection}-wise
___

## License

UDUK™ Free as an AIR License
