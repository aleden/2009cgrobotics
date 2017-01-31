#include "Driver.h"
#include "WPILib.h"
#include "HAL.h"

Driver* const driver = new Driver();

void Driver::updateTune()
{
#define SETALL(pwr) drive(0,pwr,0);
/*#define GETENC \
	frStart = hal->getFREncoder();\
	flStart = hal->getFLEncoder();\
	brStart = hal->getBREncoder();\
	blStart = hal->getBLEncoder();*/
#define GETEND(prefix) \
	    prefix ## frDelta = abs(hal->getFREncoder() /*- frStart*/ ),\
	    prefix ## flDelta = abs(hal->getFLEncoder() /*- flStart*/ ),\
	    prefix ## brDelta = abs(hal->getBREncoder() /*- brStart*/ ),\
	    prefix ## blDelta = abs(hal->getBLEncoder() /*- blStart*/ );
#define MINIMUM(a,b) ((a) > (b) ? (b) : (a))
	enum tuneMode { tuneIdle, tuneRev, tuneForward, tunePause, tuneRevBackward, tuneBackward, tuneStop };
	static tuneMode mode = tuneIdle;
	static Timer timer;
	//static int frStart, flStart, brStart, blStart;
	static int posfrDelta, negfrDelta,
			   posflDelta, negflDelta,
			   posbrDelta, negbrDelta,
			   posblDelta, negblDelta;
	const float tunePwr = 1.0;
	const float revTime = 1.5;
	const float driveTime = 2.5;
	if (hal->lStick.GetTop() && hal->rStick.GetTop()) {
		//printf("mec tune");
		switch (mode) {
		case tuneIdle:
			printf("starting idle!\n");
			timer.Start();
			mode = tuneRev;
			break;
		case tuneRev:
			SETALL(tunePwr);
			if (timer.Get() > revTime) {
				timer.Reset();
				mode = tuneForward;
				//GETENC
				hal->resetEncoders();
				/*printf ("Got rev encoders:\n");
				printf ("FR: %i\n", frStart);
				printf ("FL: %i\n", flStart);
				printf ("BR: %i\n", brStart);
				printf ("BL: %i\n", blStart);*/
				printf("Done reving up\n");
			}
			break;
		case tuneForward:
			SETALL(tunePwr);
			if (timer.Get() > driveTime) {
				timer.Reset();
				mode = tunePause;
				GETEND(pos)
				printf ("Forward:\n");
				printf ("FR: %i\n", posfrDelta);
				printf ("FL: %i\n", posflDelta);
				printf ("BR: %i\n", posbrDelta);
				printf ("BL: %i\n", posblDelta);
				printf("\n");
			}
			break;
		case tunePause:
			SETALL(0.0);
			if (timer.Get() > 1.5) {
				timer.Reset();
				mode = tuneRevBackward;
			}
			break;
		case tuneRevBackward:
			SETALL(-tunePwr);
			if (timer.Get() > revTime) {
				timer.Reset();
				mode = tuneBackward;
				//GETENC
				hal->resetEncoders();
				printf("Done reving up\n");
			}
			break;
		case tuneBackward:
			SETALL(-tunePwr);
			if (timer.Get() > driveTime) {
				timer.Reset();
				mode = tuneStop;
				GETEND(neg)
				float m = (float) MINIMUM(
						MINIMUM(MINIMUM(posfrDelta,negfrDelta),
								MINIMUM(posflDelta,negflDelta)),
						MINIMUM(MINIMUM(posbrDelta,negbrDelta),
								MINIMUM(posblDelta,negblDelta)));
				printf ("Backward:\n");
				printf ("FR: %i\n", negfrDelta);
				printf ("FL: %i\n", negflDelta);
				printf ("BR: %i\n", negbrDelta);
				printf ("BL: %i\n", negblDelta);
				printf("\n");
				printf("=================\n");
				printf("TUNING CONSTANTS:\n");
				printf("=================\n");
#define GETTUNECONST(var) (((float)m) / ((float)var))
				printf("frPos = %f\n", GETTUNECONST(posfrDelta));
				printf("frNeg = %f\n", GETTUNECONST(negfrDelta));
				printf("flPos = %f\n", GETTUNECONST(posflDelta));
				printf("flNeg = %f\n", GETTUNECONST(negflDelta));
				printf("brPos = %f\n", GETTUNECONST(posbrDelta));
				printf("brNeg = %f\n", GETTUNECONST(negbrDelta));
				printf("blPos = %f\n", GETTUNECONST(posblDelta));
				printf("blNeg = %f\n", GETTUNECONST(negblDelta));
				float avg = (((float)posfrDelta)+
						     ((float)negfrDelta)+
						     ((float)posflDelta)+
						     ((float)negflDelta)+
						     ((float)posbrDelta)+
						     ((float)negbrDelta)+
						     ((float)posblDelta)+
						     ((float)negblDelta)) / 8.0;
#define SQUARE(X) ((X) * (X))
				// standard deviation-ish thingy
				float deviance = SQUARE(posfrDelta-avg)+
					           SQUARE(negfrDelta-avg)+
					           SQUARE(posflDelta-avg)+
					           SQUARE(negflDelta-avg)+
					           SQUARE(posbrDelta-avg)+
					           SQUARE(negbrDelta-avg)+
					           SQUARE(posblDelta-avg)+
					           SQUARE(negblDelta-avg);
				
				printf("deviance = %f\n",deviance);
			}
			break;
		case tuneStop:
			SETALL(0.0);
		}
	} else {
		timer.Reset();
		timer.Stop();
		mode = tuneIdle;
	}
}

void Driver::updateDriver()
{
	static float beginStrafeAngle;
	static bool wasStrafing = false;
	
	updateTune();
	
	bool isStrafing = r == 0 && (y != 0 || x != 0);
	float strafeAngle = hal->getGyroValue();
	
	if (isStrafing && !wasStrafing)
		beginStrafeAngle = strafeAngle;
#if 0
	if (isStrafing && !wasStrafing && enableGyroCorrect) {
		float delta = strafeAngle - beginStrafeAngle;
		const float k = 0.05;
		const float maxCorrect = 0.4;
		float correct = delta * k;
		if (correct > maxCorrect) correct = maxCorrect;
		if (correct < -maxCorrect) correct = -maxCorrect;
		r += correct;
	}
#endif
	float fl = y + x + r;
	float fr = y - x - r;
	float bl = y - x + r;
	float br = y + x - r;
#if 1
	//if (hal->rStick.GetTrigger()) {
		// mecanum tuning
		#define TUNE_MTR(MTR) if (MTR > 0) MTR *= MTR ## P; else if (MTR < 0) MTR *= MTR ## N;
		TUNE_MTR(fr)
		TUNE_MTR(fl)
		TUNE_MTR(br)
		TUNE_MTR(bl)
	//}
#endif
	
    // The most awesome mecanum code, written by Tony and Stanford and Rohan
	static int tmr = 0;
	if (tmr++ > 5) {
		tmr=0;
		//printf("%f %f %f %f\n",fr,fl,br,bl);
	}
    hal->setFLPower(fl);
    hal->setFRPower(fr);
    hal->setBLPower(bl);
    hal->setBRPower(br);
    
    wasStrafing = isStrafing;
}
