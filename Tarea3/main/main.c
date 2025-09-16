 # include "esp_timer.h"
 # include "lookuptable.h"
 # include <stdio.h>
 #include <stdint.h>

 int regression_func ( int x ) {
 // Polynomial approximation ( replace with HWK2 regression )
 return ( int ) (-5.79212877e-06* x * x + 4.93648274e-02* x ) ;
 }

 void app_main ( void ) {
 int test_val = 1234;
 int result ;

 // Regression timing
 int64_t start = esp_timer_get_time () ;
 result = regression_func ( test_val ) ;
 int64_t end = esp_timer_get_time () ;
 printf ( " Regression result =%d time =%lld us \n " , result, ( end - start ) ) ;
 // Lookup timing
 start = esp_timer_get_time () ;
 result = lookup_table [ test_val ];
 end = esp_timer_get_time () ;
 printf ( " Lookup result =%d time =%lld us \n " , result , (end - start ) ) ;
 }