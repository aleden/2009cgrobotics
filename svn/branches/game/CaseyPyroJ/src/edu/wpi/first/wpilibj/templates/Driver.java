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
public class Driver implements Part {

    //mecanum tuning constants
    private double frP = 0.976161;
    private double frN = 0.976641;
    private double flP = 0.927171;
    private double flN = 0.888988;
    private double brP = 1.000000;
    private double brN = 0.980499;
    private double blP = 0.997739;
    private double blN = 0.967600;
    
    private double x;
    private double y;
    private double r;
    
    public boolean enableGyroCorrect;

    private double flstart;
    private double frstart;
    private double blstart;
    private double brstart;

    private double frdelta;
    private double fldelta;
    private double brdelta;
    private double bldelta;

    private double negfrdelta;
    private double negfldelta;
    private double negbrdelta;
    private double negbldelta;

    int tmr;
    
    public Driver()
    {
        x = y = r = 0;
        tmr = 0;
    }

    public void runDiagnostics(PyroMain main){}

    public void drive(double x, double y, double r)
    {
        this.x = x;
        this.y = y;
        this.r = r;
    }

    public void setX(double x) { this.x = x; }
    public void setY(double y) { this.y = y; }
    public void setR(double r) { this.r = r; }
    public double getX() { return x; }
    public double getY() { return y; }
    public double getR() { return r; }

    public void initAutonomous(PyroMain main)
    {
        x = y = r = 0;
    }
    public void initOperator(PyroMain main)
    {
        x = y = r = 0;
    }
    public void runAutonomous(PyroMain main)
    {
        updateDriver(main);
    }
    public void runOperator(PyroMain main)
    {
        updateDriver(main);
    }

    private void SETALL(double pwr) { drive(0,pwr,0); }
    private double SQUARE(double X) { return (X*X); }
    private double MINIMUM(double X,double Y) { return Math.min(X,Y); }
    private double GETTUNECONST(double m,double var) { return (((double)m) / ((double)var)); }
    /*private void GETEND(PyroMain main)
    {
        frdelta = Math.abs(main.hal.getFREncoder() - frStart );
        fldelta = Math.abs(main.hal.getFLEncoder() - flStart );
        brdelta = Math.abs(main.hal.getBREncoder() - brStart );
        bldelta = Math.abs(main.hal.getBLEncoder() - blStart );
    }
/*
    public void updateTune(PyroMain main)
    {
        //enum tuneMode { tuneIdle, tuneRev, tuneForward, tunePause, tuneRevBackward, tuneBackward, tuneStop };
        int mode = 0;
	Timer timer = new Timer();
	//static int frStart, flStart, brStart, blStart;
	double tunePwr = 1.0;
	double revTime = 1.5;
	double driveTime = 2.5;
	if (main.hal.lStick.getTop() && main.hal.rStick.getTop()) {
		//System.out.println("mec tune");
		switch (mode) {
		case 0:
			System.out.println("starting idle!\n");
			timer.start();
			mode = 1;
			break;
		case 1:
			SETALL(tunePwr);
			if (timer.get() > revTime) {
				timer.reset();
				mode = 2;
				//getENC
				main.hal.resetEncoders();
				System.out.println ("Got rev encoders:\n");
				System.out.println ("FR: %i\n", frStart);
				System.out.println ("FL: %i\n", flStart);
				System.out.println ("BR: %i\n", brStart);
				System.out.println ("BL: %i\n", blStart);
				System.out.println("Done reving up\n");
			}
			break;
		case 2:
			SETALL(tunePwr);
			if (timer.get() > driveTime) {
				timer.reset();
				mode = 3;
				GETEND(main);
				System.out.println ("Forward:\n");
                                //format is fr,fl,br,bl
				System.out.println(frdelta);
				System.out.println (fldelta);
				System.out.println (brdelta);
				System.out.println (bldelta);
				System.out.println("\n");
			}
			break;
		case 3:
			SETALL(0.0);
			if (timer.get() > 1.5) {
				timer.reset();
				mode = 4;
			}
			break;
		case 4:
			SETALL(-tunePwr);
			if (timer.get() > revTime) {
				timer.reset();
				mode = 5;
				//getENC
				main.hal.resetEncoders();
				System.out.println("Done reving up\n");
			}
			break;
		case 5:
			SETALL(-tunePwr);
			if (timer.get() > driveTime) {
				timer.reset();
				mode = 6;
				GETEND(main);
				double m = (double)MINIMUM(MINIMUM(MINIMUM(frdelta,negfrdelta),MINIMUM(fldelta,negfldelta)),MINIMUM(MINIMUM(brdelta,negbrdelta),MINIMUM(bldelta,negbldelta)));
				System.out.println ("Backward:\n");
				System.out.println (negfrdelta);
				System.out.println (negfldelta);
				System.out.println (negbrdelta);
				System.out.println (negbldelta);
				System.out.println("\n");
				System.out.println("=================\n");
				System.out.println("TUNING CONSTANTS:\n");
				System.out.println("=================\n");
				System.out.println(GETTUNECONST(frdelta,m));
				System.out.println(GETTUNECONST(negfrdelta,m));
				System.out.println(GETTUNECONST(fldelta,m));
				System.out.println(GETTUNECONST(negfldelta,m));
				System.out.println(GETTUNECONST(brdelta,m));
				System.out.println(GETTUNECONST(negbrdelta,m));
				System.out.println(GETTUNECONST(bldelta,m));
				System.out.println(GETTUNECONST(negbldelta,m));
				double avg = (((double)frdelta)+
						     ((double)negfrdelta)+
						     ((double)fldelta)+
						     ((double)negfldelta)+
						     ((double)brdelta)+
						     ((double)negbrdelta)+
						     ((double)bldelta)+
						     ((double)negbldelta)) / 8.0;
				// standard deviation-ish thingy
				double deviance = SQUARE(frdelta-avg)+
					           SQUARE(negfrdelta-avg)+
					           SQUARE(fldelta-avg)+
					           SQUARE(negfldelta-avg)+
					           SQUARE(brdelta-avg)+
					           SQUARE(negbrdelta-avg)+
					           SQUARE(bldelta-avg)+
					           SQUARE(negbldelta-avg);
                                System.out.println("Deviance:");
				System.out.println(deviance);
			}
			break;
		case 6:
			SETALL(0.0);
		}
	} else {
		timer.reset();
		timer.stop();
		mode = 0;
	}
    }
*/
    public void updateDriver(PyroMain main)
    {
        //double beginStrafeAngle;
	//boolean wasStrafing = false;

	//updateTune(main);

	boolean isStrafing = r == 0 && (y != 0 || x != 0);
	//double strafeAngle = main.hal.getGyroValue();

	//if (isStrafing && !wasStrafing)
            //beginStrafeAngle = strafeAngle;

        double fl = y + x + r;
	double fr = y - x - r;
	double bl = y - x + r;
	double br = y + x - r;

        /*#define TUNE_MTR(MTR) if (MTR > 0) MTR *= MTR ## P; else if (MTR < 0) MTR *= MTR ## N;
		TUNE_MTR(fr)
		TUNE_MTR(fl)
		TUNE_MTR(br)
		TUNE_MTR(bl)*/
        if (fr > 0)
            fr *= frP;
        else
            fr *= frN;
        if (fl > 0)
            fl *= flP;
        else
            fl *= flN;
        if (br > 0)
            br *= brP;
        else
            br *= brN;
        if (bl > 0)
            bl *= blP;
        else
            bl *= blN;

	if (tmr++ > 5) {
            tmr=0;
            //printf("%d %d %d %d\n",fr,fl,br,bl);
	}

        main.hal.setFLPower(fl);
        main.hal.setFRPower(fr);
        main.hal.setBLPower(bl);
        main.hal.setBRPower(br);

        //wasStrafing = isStrafing;
    }
}
