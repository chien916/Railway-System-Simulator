/**
 * signal light
 * 
 * i/o from ctc:
 * -> connection[0] -> connection to track model
 * -> ctcMaintanceMode[1]
 * -> suggested speed from CTC[2:9] -> USED AS OF CTC MAINTANANCE MODE
 * -> RESERVED[10]
 * -> authority direction [11:12]
 * -> authority block numbers [13:20]
 * -> switch position from CTC[21:22] //00-auto 01-bottom 11-top
 * 
 * -> curr occupancy[23]  -> do nothing
 * -> failure mode[24:25]  -> do nothing
 * -> trackControllerMaintanceMode[26]
 * -> wayside connected to train model[27] ?
 * -> wayside connected to beacon[28] ?
 *
 * 
 * inputs from track controller
 * -> connection[0]
 * -> maintanacdeMode[1]
 * -> manual commanded speed[2:9]
 * -> left signal[10:11] //CAN BE OVERWRITTEN UNDER MAINTANCE MODE
 * -> RESERVED[12]
 * -> gate [13] //CAN BE OVERWRITTEN UNDER MAINTANCE MODE
 * -> right signal [14:15] //CAN BE OVERWRITTEN UNDER MAINTANCE MODE
 * -> authority direction [16:17] //CAN BE OVERWRITTEN UNDER MAINTANCE MODE
 * -> authority block numbers [18:25] //CAN BE OVERWRITTEN UNDER MAINTANCE MODE
 * -> switch position from kc [26:27] //00-auto 01-bottom 11-top //CAN BE OVERWRITTEN UNDER MAINTANCE MODE
 * 
 * 
 * i/o from track model (db):
 * -> connection[0] -> connection to track model
 * -> prevprev occupancy[1] 
 * -> prev occypancy[2] 
 * -> curr occupancy[3] 
 * -> next occupancy[4] 
 * -> nextnext occupancy[5] 
 * -> broken rail[6]
  * ->Track Circuit[7]
 * -> speed limit[8:15]
 * 
 * -> left signal[16:17]
 * -> switch [18]
 * -> gate [19]
 * -> right signal [20:21]
 * 
 * -> can travel to prev[22]
 * -> can travel to next[23]
 * -> left has switch[24]
 * -> right has switch[25]
 * -> station on the left[26]
 * -> station on the right[27]
 * -> is underground[28]
 * -> has crossing gate[29]
 * 
 * -> pwoer failure[30]
 * 
 * outputs to beacon
 * -> authority direction [0:1]  AS LONG AS CONNECTIONS OK -> pass to beacon -> do nothing
 * -> authority block numbers[2:9]  AS LONG AS CONNECTIONS OK -> pass to beacon -> do nothing
 * -> commanded speed[10:17]
 * -> station on the left[26]
 * -> station on the right[27]
 * -> is underground[28]
  */

// let _CTCPLCIO =
//     ("00000000") +
//     ("00000000") +
//     ("00000000") +
//     ("00000000");
// let _BCNPLCOUT =
//     ("00000000") +
//     ("00000000") +
//     ("00000000") +
//     ("00000000");
// let _KMPLCIO =
//     ("10000011") +
//     ("00000000") +
//     ("00000000") +
//     ("00000000");
// let _KCPLCIN =
//     ("00000000") +
//     ("00000000") +
//     ("00000000") +
//     ("00000000");

// let CTCPLCIO = [..._CTCPLCIO].map((it_s) => it_s === "1");
// let BCNPLCOUT = [..._BCNPLCOUT].map((it_s) => it_s === "1");
// let KMPLCIO = [..._KMPLCIO].map((it_s) => it_s === "1");
// let KCPLCIN = [..._KCPLCIN].map((it_s) => it_s === "1");
// plc(CTCPLCIO, BCNPLCOUT, KMPLCIO, KCPLCIN);
// let i = 0;

function plc($CTC_IO, $KC_IN, $KM_IO, $BCN_OUT) {


    // $CTC_OUT[0] = $KM_IO[0] & //IF TRACK MODEL IS CONNECTED
    //     $KM_IO[3];//PASS THE CURRENT OCCUPANCY OF TRACK MODEL TO CTC, 
    // //0 IF CONNECTION LOST

    for (let $_ = 1; $_ >= 0; --$_)
        $CTC_IO[24 + $_] = 
            $KM_IO[0] && //IF TRACK MODEL IS CONNECTED
            $KM_IO[6 + $_];//PASS THE FAILURE MODE OF TRACK MODEL TO CTC, 0 IF CONNECTION LOST
    //0 IF CONNECTION LOST


    $CTC_IO[26] =
        $KC_IN[0]  //IF WAYSIDE IS CONNECTED
        && $KC_IN[1];//PASS THE MAINTANCE MODE OF WAYSIDE TO CTC
    // 0 IF CONNECTION LOST

    for (let $_ = 1; $_ >= 0; --$_) $BCN_OUT[0 + $_] =
        ($CTC_IO[0] && $CTC_IO[1]) ?////IF CTC IS CONNECTED AND UNDER MAINTANANCE MODE
            $CTC_IO[11 + $_] :// PASS THE AUTHORITY DIRECTION FROM CTC TO BEACON
            ($KC_IN[16 + $_]);//OTHERWISE PASS THE AUTHORITY DIRECTION FROM WAYSIDE TO TRACK CONTROLLER
    //0 IF BOTH CONNECTIONS ARE LOST

    for (let $_ = 1; $_ >= 0; --$_) $BCN_OUT[0 + $_] =
        ($CTC_IO[0] && $CTC_IO[1]) ?//IF CTC IS CONNECTED AND UNDER MAINTANANCE MODE
            $CTC_IO[13 + $_] :// PASS THE AUTHORITY BLOCK COUNT FROM CTC TO BEACON
            ($KC_IN[18 + $_]);//OTHERWISE PASS THE AUTHORITY DIRECTION FROM WAYSIDE TO TRACK CONTROLLER
    //0 IF BOTH CONNECTIONS ARE LOST

    /**
     * CONCEPT OF DYNAMIC PROGRAMMING --> PREFIX FIRST BIT FIND
     * FIND THE FIRST BIT COUNTING FROM THE LEFT FROM SPEED LIMIT, SET ALL SUBSEQUENT BITS RIGHT OF THAT BIT TO ONE]
     * FOR EXAMPLE 0B01001001 OR 0B01111111 OR 0B01000000 GENERATES A MAXIMUM COMMANDED SPEED OF 0B00111111
     * THIS STEP WILL LEAVE BCN_OUT[10:17] THE MAXIMUM COMMANDED SPEED
     * IF BOTH CTC AND TRACK MODEL IS UNCONNECTED, THE BLOCK COUNT IS 0, AND WILL RESET THE FIELD HERE TO 0 AS WELL
     */
    for (let $_ = 1; $_ <= 7; ++$_) $BCN_OUT[10 + $_] =
        $BCN_OUT[10 + $_ - 1] || $KM_IO[8 + $_ - 1];

    for (let $_ = 5; $_ >= 0; --$_)  $BCN_OUT[10 + $_] =
        ($KC_IN[0] & $KC_IN[0]) ?//IF WAYSIDE IS CONNECTED AND UNDER MAINTANANCE MODE
            $KC_IN[2 + $_] :// PASS THE MANUAL COMMANDED SPEED FROM WAYSIDE TO BEACON
            ($CTC_IO[0] && $CTC_IO[0]) ?//ELSE IF CTC IS CONNECTED AND UNDER MAINTANANCE MODE
                $CTC_IO[2 + $_] : //PASS THE SUGGESTED SPEED FROM CTC TO BEACON
                ($BCN_OUT[10 + $_] && $BCN_OUT[2 + $_ + 2]);//OTHERWISE, CALCULATE MIN((NUMBER OF BLOCKS AUTHORIZED)*4,SPEED LIMIT FROM TRACK MODEL) AS OPTIMIZED COMMANDED SPEED TO BEACON

    //COPY CONSTANT INFRASTRUCTURE INFORMATION TO BEACON FOR TRAIN TO READ
    $BCN_OUT[26] = $KM_IO[26]
    $BCN_OUT[27] = $KM_IO[27]
    $BCN_OUT[28] = $KM_IO[28]
    $BCN_OUT[29] = $KM_IO[29]

    //KM::LEFT_SIGNAL
    $KM_IO[16] =
        ($KC_IN[0] && $KC_IN[0]) ?//IF WAYSIDE IS CONNECTED AND UNDER MAINTANANCE MODE
            $KC_IN[10] ://USE THE MANUAL LEFT SIGNAL FROM WAYSIDE
            !$KM_IO[1] && !$KM_IO[2];
    $KM_IO[17] =
        ($KC_IN[0] && $KC_IN[0]) ?//IF WAYSIDE IS CONNECTED AND UNDER MAINTANANCE MODE
            $KC_IN[11] ://USE THE MANUAL LEFT SIGNAL FROM WAYSIDE
            $KM_IO[1] && !$KM_IO[2];

    $KM_IO[18] = //NEW SWITCH POSITION FOR TRACK MODEL
        ($KC_IN[0] && $KC_IN[1] && $KC_IN[27]) ?//IF WAYSIDE IS CONNECTED AND UNDER MAINTANANCE MODE
            $KC_IN[26] ://USE THE MANUAL SWITCH POSITION FROM WAYSIDE
            ($CTC_IO[0] && $CTC_IO[0] && $CTC_IO[22]) ?//ELSE IF CTC IS CONNECTED AND UNDER MAINTANANCE MODE
                $CTC_IO[21] ://USE THE MANUAL SWITCH POSITION FROM CTC
                $BCN_OUT[1];//OTHERWISE, DEDUCE SWITCH POSITION FROM THE AUTHORITY DIRECTION


    //KM::CROSSING_GATE
    $KM_IO[19] =//NEW CROSSING GATE POSITION FOR TRACK MODEL
        ($KC_IN[0] && $KC_IN[0]) ?//IF WAYSIDE IS CONNECTED AND UNDER MAINTANANCE MODE
            $KC_IN[13] ://USE THE MANUAL GATE POSITION FROM WAYSIDE
            $KM_IO[1] || $KM_IO[2] || $KM_IO[3] || $KM_IO[4] || $KM_IO[5];//OTHERWISE, GATE IS CLOSED IF TRAIN OCCUP WITHIN 2 BLOCKS

    //KM::RIGHT_SIGNAL
    $KM_IO[20] =
        ($KC_IN[0] && $KC_IN[0]) ?//IF WAYSIDE IS CONNECTED AND UNDER MAINTANANCE MODE
            $KC_IN[14] ://USE THE MANUAL GATE POSITION FROM WAYSIDE
            !$KM_IO[4] && !$KM_IO[5];
    $KM_IO[21] =
        ($KC_IN[0] && $KC_IN[0]) ?//IF WAYSIDE IS CONNECTED AND UNDER MAINTANANCE MODE
            $KC_IN[15] ://USE THE MANUAL GATE POSITION FROM WAYSIDE
            !$KM_IO[4] & $KM_IO[5];

}


