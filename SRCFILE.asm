COPY	START   0
FIRST   LDX     ZERO
MOVE    LDCH    STR1,X 
        STCH    STR2,X  
        TIX     LEN 
        JLT     MOVE       
STOP    J       STOP        
STR1    BYTE    C'HELLO'    
STR2    RESB    5          
ZERO    WORD    0           
LEN     WORD    5           
        END     FIRST
