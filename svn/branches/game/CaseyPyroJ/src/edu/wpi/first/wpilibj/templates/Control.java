/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
// WHY don't they have enums in Java ME?????

package edu.wpi.first.wpilibj.templates;

import edu.wpi.first.wpilibj.*;

/**
 *
 * @author Currey-WilsonC
 */
public class Control implements Part
{
    //0=mode0,1=mode1,2=mode2
    public int mode;

    private int dtmr; //for disabled mode

    boolean lastB6 = false;
    boolean lastB7 = false;
    boolean lastB11 = false;
    boolean lastB10 = false;


    public Control()
    {
        dtmr = 0;
    }

    private boolean BETWEEN(double START, double END, double VAL) {return ((VAL) >= (START) && (VAL) <= (END));}

    public void initAutonomous(PyroMain main)
    {
        //main.locator.reset(main);
    }

    public void initOperator(PyroMain main)
    {

    }

    public int getAutoMode()
    {
        return mode;
    }

    public void runAutonomous(PyroMain main)
    {
	Timer tmr = new Timer();
	boolean inited = false;
	if (!inited)
        {
            inited = true;
            tmr.start();
	}
    }

    public void runOperator(PyroMain main) {
        double x = main.hal.lStick.getX();
        double y = -(main.hal.lStick.getY());
        double r = main.hal.rStick.getX();
        main.driver.drive(deadzone(x),deadzone(y),deadzone(r));

        
        main.sucker.stop();
	if (main.hal.lStick.getTrigger())
            main.sucker.blow();
	if (main.hal.rStick.getTrigger())
            main.sucker.suck();
        /*
	double bottom = 6;
	double top = 12.5; //(hal.lStick.GetThrottle() + 1.0) / 2.0 * 5.0 + 10.0;
	double range = top - bottom;

        main.script.setKickWinchThreshold(((-main.hal.hStick.getThrottle()) + (double)1) / (double)2 * range + bottom);

	// KICKING + AUTOCOCK
	Timer autocockTimer = new Timer();
	boolean hasInitedAutocock = false;
	if (!(hasInitedAutocock)) {
            hasInitedAutocock = true;
            autocockTimer.start();
	}
	if (main.hal.rStick.getTop() || main.hal.hStick.getTrigger()) {
		main.hal.setKickerLatch(true);
		main.sucker.blow();
		autocockTimer.reset();
	} else {
            if (autocockTimer.get() > 0.3)
                main.script.runKickScript();
	}
	if (main.hal.hStick.getRawButton(6))
            main.script.abortScript(main);
         */
        lastB6 = false;
        lastB7 = false;
        lastB11 = false;
        lastB10 = false;
	boolean newB6 = main.hal.lStick.getRawButton(6);
	boolean newB7 = main.hal.lStick.getRawButton(7);
	boolean newB10 = main.hal.lStick.getRawButton(10);
	boolean newB11 = main.hal.lStick.getRawButton(11);

	if (newB6 && newB7 && newB10 && newB11 && (!lastB6 || !lastB7 || !lastB10 || !lastB11)) {
            main.setDiagnosticsModeEnabled(true);
	}
	lastB6 = newB6;
	lastB7 = newB7;
	lastB10 = newB10;
	lastB11 = newB11;

         
    }

    public void runDiagnostics(PyroMain main)
    {
            boolean newB6 = main.hal.rStick.getRawButton(6),
            newB7 = main.hal.rStick.getRawButton(7),
            newB10 = main.hal.rStick.getRawButton(10),
            newB11 = main.hal.rStick.getRawButton(11);

            if (newB6 && newB7 && newB10 && newB11 &&
                    (!lastB6 || !lastB7 || !lastB10 || !lastB11))
            {
                main.setDiagnosticsModeEnabled(false);
            }
            lastB6 = newB6;
            lastB7 = newB7;
            lastB10 = newB10;
            lastB11 = newB11;
    }

    public void runDisabled(PyroMain main)
    {
            //static const char* automodeStrings[] = { "mode0", "mode1", "mode2" };

            double lThrottle = (main.hal.lStick.getThrottle() + 1.0)/2.0;

            if (BETWEEN(0.0f, 0.2f, lThrottle))
                    mode = 0;
            else if (BETWEEN(0.4f, 0.6f, lThrottle))
                    mode = 1;
            else if (BETWEEN(0.8f, 1.0f, lThrottle))
                    mode = 2;

            if (dtmr++ > 10000)
            {
                    dtmr = 0;
                    //printf("mode = %s\n", automodeStrings[mode]);
            }
    }

    double deadzone(double x) {
        return deadzone(x,0.15);
    }

    double deadzone(double x, double radius) {
        if (Math.abs(x) < radius) {
            return 0.0;
        }
        return x;
    }

    /*void runMidAuto(PyroMain main)
    {
        //0=initialize, 1=waitForCock, 2=goForward, 3=kickFollowthrough, 4=waitForCock2, 5=goForward2, 6=kickFollowthrough2, 7=done;
        int state = 0;
        double speed = 0.3;
        double distanceToFirstBall = 24;
        double distanceToSecondBall = 95 + 4 + 7;
        double followthroughDistance = 6;
        Timer tmr = new Timer();
        switch (state) {
            case 0:
                tmr.start();
                state = 1;
                break;
            case 1:
                main.script.runKickScript();
                main.script.setKickWinchThreshold(12);
                main.driver.drive(0,0,0);
                if (tmr.get() > 4.5)
                    state = 2;
                break;
            case 2:
                main.driver.drive(0.0,speed,0.0);
                main.sucker.suck();
                main.script.runKickScript();
                main.script.setKickWinchThreshold(12);
                if (main.locator.getX() > distanceToFirstBall) {
                    //printf("Got to first ball");
                    main.hal.setKickerLatch(true);
                    main.sucker.blow();
                    state = 3;
                }
                break;
            case 3:
                main.driver.drive(0.0,speed * 0.7,0.0);
                main.sucker.blow();
                main.hal.setKickerLatch(true);
                if (main.locator.getX() > distanceToFirstBall + followthroughDistance) {
                    //pritnf("followed through");
                    state = 4;
                    tmr.reset();
                }
                break;
            case 4:
                main.driver.drive(0.0,0.0,0.0);
                main.sucker.stop();
                main.script.runKickScript();
                main.script.setKickWinchThreshold(12);
                if (tmr.get() > 3.5) {
                    state = 5;
                    //printf("7 cocking");
                }
                break;
            case 5:
                //double dy = locator.getY();
                //double ky = -0.1;
                double rspeed = -0.05 / 3.0; //speed / 4;
                main.driver.drive(0,speed,rspeed);
                main.sucker.suck();
                main.script.runKickScript();
                main.script.setKickWinchThreshold(12);
                if (main.locator.getX() > distanceToSecondBall) {
                    main.hal.setKickerLatch(true);
                    main.sucker.blow();
                    state = 6;
                }
                break;
            case 6:
                main.driver.drive(0.0,speed,0.0);
                main.sucker.blow();
                main.hal.setKickerLatch(true);
                if (main.locator.getX() > distanceToSecondBall + followthroughDistance) {
                    state = 7;
                }
                break;
            case 7:
                main.script.abortScript(main);
                main.hal.setKickerLatch(true);
                main.driver.drive(0.0,0.0,0.0);
                main.sucker.stop();
                break;
        }
    }*/

    /*void runDefAuto(PyroMain main)
    {
        // Go forward while running intake.
        // If voltage required to reach current drops below some threshold, kick
        double speed = 0.6;
        double voltageThreshold = -0.3;

        main.sucker.suck();
        main.driver.drive(0.0,speed,0.0);

        if (main.sucker.getVoltage() > voltageThreshold)
                main.hal.setKickerLatch(true);

        main.script.setKickWinchThreshold(10.5);
        main.script.runKickScript();
    }*/

    /*void runScoreAuto(PyroMain main)
    {
        //enum autostate { 0=2, 1=kickBall, 2=moveAway, 3=7Auto };
        int state = 0;
        double distanceToBall = 95;
        double followthroughDistance = 6; //12;
        double minX = 0; //18;
        double maxY = 3 * 12; //12 * (27 - 5);
        double speed = 1.0;
        double rotSpeed = -0.06; // turn with a curve
        double retreatDistance = 14;
        //const double moveAwayStrafe = 0.3;
        switch (state) {
            case 0:
                // cock kicker
                main.script.runKickScript();
                main.script.setKickWinchThreshold(9);
                main.driver.drive(0.0,speed * 0.6,rotSpeed * 0.6);
                main.sucker.suck();
                if (main.locator.getX() > distanceToBall) {
                    state = 1;
                    System.out.println("7 going forward\n");
                }
                break;
            case 1:
                main.driver.drive(0.0,speed * 0.6,0.0);
                main.hal.setKickerLatch(true);
                main.sucker.suck();
                if (main.locator.getX() > distanceToBall + followthroughDistance) {
                    state = 2;
                    System.out.println("7 following through\n");
                }
                break;
            case 2:
                main.sucker.stop();
                main.driver.drive(0,-speed,-rotSpeed * 3); // or -1.0, which is it? i think it's positive
                if (main.locator.getX() < retreatDistance) {
                    state = 3;
                    System.out.println("7 moving away\n");
                }
                break;
            case 3:
                main.driver.drive(0.0,0.0,0.0);
                break;
        }
    }*/
}
