class zCTimer {
    float factorMotion;        //zREAL        //nicht zu klein machen. Sonst: Freeze bei hoher Framerate!
    float frameTimeFloat;      //zREAL [msec] //Zeit der zwischen diesem und dem letzten Frame verstrichen ist
    float totalTimeFloat;      //zREAL [msec] //gesamte Zeit
    float frameTimeFloatSecs;  //zREAL  [s]
    float totalTimeFloatSecs;  //zREAL  [s]
    int lastTimer;           //zDWORD
    int frameTime;           //zDWORD [msec] //nochmal als Ganzahl
    int totalTime;           //zDWORD [msec]
    int minFrameTime;        //zDWORD       //antifreeze. Sonst wird die Framezeit auf 0 gerundet und nichts bewegt sich
    int forcedMaxFrameTime;  //zDWORD //l�nger als das darf ein Frame (in Spielzeit) nicht dauern. Um zu gro�e Zeitspr�nge f�r die Objekte zu vermeiden? Jedenfalls sort dies daf�r, dass das Spiel langsamer l�uft, wenn das Spiel mit rendern nicht hinterherkommt.
};
