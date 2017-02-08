# bignum_calc


typedef struct {
        int digits[max_length];        
	      int sign;			/* 1 if positive, -1 if negative */ 
        int decimal_point;			/* index of high-order digit */( 85.4 -> 2)
} bignum;

Split -  +, -, *, /, sqrt(), pow() and log(), ""Flex and Bison - together""

