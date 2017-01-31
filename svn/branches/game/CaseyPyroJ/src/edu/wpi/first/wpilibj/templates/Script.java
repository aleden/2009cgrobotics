/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.wpi.first.wpilibj.templates;

import edu.wpi.first.wpilibj.*;

/**
 *
 * @author Currey-WilsonC
 */
public class Script implements Part {

    private int currentScript;
    //{1=bumpID, 2=kickerID, 3=winchID, 0=none};

    private int scriptPosition;
    //{0=0, 1=1, 2=2, 3=3, 4=4, 5=5, 6=6 };

    private double oldTimerValue;
    private double kickWinchThreshold;
    private boolean kickerScriptRunning;

    public void runDiagnostics(PyroMain main){}

    public void setScriptPosition(int pos)
    {
        scriptPosition = pos;
    }

    public int getScriptPosition()
    {
        return scriptPosition;
    }

    public void initAutonomous(PyroMain main)
    {
        currentScript = 0;
    }

    public void initOperator(PyroMain main)
    {
        currentScript = 0;
    }

    public void runAutonomous(PyroMain main)
    {
        if (kickerScriptRunning){
            currentScript = 2;
	}
	else{
            currentScript = 0;
	}
	int id = currentScript;
	runScripts(main,id);
    }

    public void runOperator(PyroMain main)
    {
        int id = currentScript;
	runScripts(main,id);
    }

    public void runScripts(PyroMain main, int id)
    {
        Timer tmr = new Timer();
        Timer timer = new Timer();
	double slidebackTime = 0.6;
	double savedThreshold = 0;
	double windBackTime = 1.5;
	double cylinderForwardTime = .3;
	if (id == 2)
	{
		int currentPosition = getScriptPosition();
		//printf("running kick script\n");
		// Steps for kicking:
		// 1. Open latch
		// 2. Push kicker forward into latch
		// 3. Close latch
		// 4. Winch kicker back until desired current is reached
		// 5. Exit (actual kicking (the unlatching) is done by the user)

		switch(currentPosition)
		{
		case 0:
			setScriptPosition(1);
			timer.start();
			timer.reset();
			//printf("STARTING KICK SCRIPT");
		case 1:
			main.hal.setKickerLatch(true); // open up latch
			main.hal.setWinchShootPower(1); // push kicker back into latch
			//printf("MOVING KICKER TO LATCH");
			if (timer.get() > windBackTime)
			{
				// kicker winch is out far enough
				main.hal.setKickerForwardValue(true);
				timer.start();
				timer.reset();
				setScriptPosition(2);
			}

			// On failure: setScriptPosition(5);
			break;

		case 2:
			if (timer.get() > cylinderForwardTime)
			{
				main.hal.setKickerLatch(false); // close latch
				main.hal.setKickerForwardValue(false); // close kicker cylinder
				setScriptPosition(3);
			}
			break;

		case 3:
		{
			double power = main.hal.getShooterCurrent();
			double stopRadius = 0.5;
			double delta = Math.abs(power - kickWinchThreshold);
			//printf("%f %f|",power,kickWinchThreshold);
			if (delta < stopRadius) {
				setScriptPosition(5);
				savedThreshold = kickWinchThreshold;
				//printf("DONE");
			} else if (power > kickWinchThreshold){
				// slide back a bit
				setScriptPosition(4);
				tmr.start();
				tmr.reset();
				main.hal.setWinchShootPower(1);
				//printf("SLIDE");
			} else {
				main.hal.setWinchShootPower(-1);
				//printf("UP");
			}
			if (main.hal.getKickerLatch()) {
				//printf("DONE COCK\n");
				setScriptPosition(0);
				currentScript = 0;
				kickerScriptRunning = false;
				main.hal.setWinchShootPower(0);
			}
		}
			break;
		case 4:
			main.hal.setWinchShootPower(1);
			if (tmr.get() > slidebackTime) {
				setScriptPosition(2);
				//printf("TM");
			}
			if (main.hal.getKickerLatch()) {
				//printf("DONE COCK\n");
				setScriptPosition(0);
				currentScript = 0;
				kickerScriptRunning = false;
				main.hal.setWinchShootPower(0);
			}
			break;
		case 5: {
			main.hal.setWinchShootPower(0);
			double stopRadius = 0.5;
			double delta = Math.abs(savedThreshold - kickWinchThreshold);
			if (delta > stopRadius)
				setScriptPosition(2); }
		if (main.hal.getKickerLatch()) {
			//printf("DONE COCK\n");
			setScriptPosition(0);
			currentScript = 0;
			kickerScriptRunning = false;
			main.hal.setWinchShootPower(0);
		}
		break;
		case 6: // latch failure, wind it back
			main.hal.setWinchShootPower(-1);
			//printf("F");
			if (main.hal.getWinchBreaker()) {

				setScriptPosition(0);
			}

			break;
		default:
			//printf("ERROR: INVALID POS FOR SCRIPT SHOOT");
			break;
		}

		return;

	}

	else
	{
            currentScript = 0;
	}
    }

    public void checkScriptButtons(PyroMain main)
    {
         boolean inKickMode = main.hal.getKickerButton();
         boolean aborted = main.hal.getScriptAbortButton();
         if (inKickMode){
             currentScript = 2;
             //printf("In kick mode");
         }
         if (aborted){
             abortScript(main);
             //printf("ABORT");
         }
    }

    public void abortScript(PyroMain main){  // for auto mode
	currentScript = 0;
	setScriptPosition(0);
	main.hal.setWinchShootPower(0.0);
    }

    public void setKickWinchThreshold(double threshold) {
	kickWinchThreshold = threshold;
    }

    public void runKickScript(){  // for auto mode
	kickerScriptRunning = true;
	currentScript = 2;
    }

    public int getCurrentScript(PyroMain main){
	checkScriptButtons(main);
	return currentScript;
    }
}
