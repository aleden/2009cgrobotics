/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.wpi.first.wpilibj.templates;

import edu.wpi.first.wpilibj.*;
//import edu.wpi.first.addons.*;

/**
 *
 * @author Currey-WilsonC
 */
public class HAL implements Part
{
    private int tmr;

    public Joystick rStick;
    public Joystick lStick;
    public Joystick hStick;

    private AnalogChannel kickerAnalogChannel;
    //Left motors currently thought to be positive=forward.
    private CANJaguar frMotor;
    private CANJaguar flMotor;
    private CANJaguar brMotor;
    private CANJaguar blMotor;
    private CANJaguar winchShootMotor;

    //Relay winchBrake;
    /*CANJaguar winchHangMotor; CANJaguar winchHangMotor2; */ // hanger is dead
    private CANJaguar suckerMotor;
    private CANJaguar suckerMotor2;
    private Compressor compressor;

    private DigitalInput kickLimit;
    private Solenoid kickerSolenoid, kickerForwardSolenoid;
    //private Encoder frEncoder, flEncoder,brEncoder, blEncoder;

    private DriverStationLCD dsLCD;
    //private Servo armServo;

    private int rStickPort = 3;
    private int lStickPort = 2;
    private int hStickPort = 1;
    private int frMotorPort = 3; private int flMotorPort = 4;
    private int brMotorPort = 9; private int blMotorPort = 2;
    //private int armRelayChannel = 1;
    private int kickerRelayChannel = 3;
    private int winchShootMotorPort = 6;

    private int suckerMotorPort2 = 7;
    private int suckerMotorPort = 8;
    //private int gyroPort = 1;
    //private int winchHangMotorPort = 7; // Retract - negative
    //private int winchHangMotor2Port = 11; // Retract - positive
    //encoder ports subject to change
    /*private int frEncoderPort1 = 3; private int frEncoderPort2 = 4;
    private int flEncoderPort1 = 7; private int flEncoderPort2 = 8;
    private int brEncoderPort1 = 5; private int brEncoderPort2 = 6;
    private int blEncoderPort1 = 1; private int blEncoderPort2 = 2;*/

    private int pressureSwitchChannel = 9;
    private int compressorRelayChannel = 1;

    private int kickerForwardSolenoidChannel = 1;
    private int kickerSolenoidChannel = 2;
    private int kickLimitPort = 10;

    private int kickerAnalogChannelPort = 3;

    private double brMotorPower;
    private double blMotorPower;
    private double frMotorPower;
    private double flMotorPower;
    private double winchShootPower;
    private double winchHangPower;
    private double suckerPower;
    private boolean kickerValue;
    private boolean kickerForwardValue;
    private boolean armValue;
    private boolean kickLimitValue;
    private boolean hangerBreakValue;
    //Relay::Value winchBrakeValue;
    private int scriptMode;

    private boolean shootWinchBreaker;
    private double shootWinchBreakerCurrent = 13;

    public int latchLockedAngle = 100, latchUnlockedAngle = 180;

    public HAL()
    {
        tmr = 0;

        rStick = new Joystick(rStickPort);
        lStick = new Joystick(lStickPort);
        hStick = new Joystick(hStickPort);

        kickerAnalogChannel = new AnalogChannel(kickerAnalogChannelPort);
        frMotor = new CANJaguar(frMotorPort);
        flMotor = new CANJaguar(flMotorPort);
        brMotor = new CANJaguar(brMotorPort);
        blMotor = new CANJaguar(blMotorPort);
        winchShootMotor = new CANJaguar(winchShootMotorPort);
        //winchBrake(winchBrakeRelayChannel),
        //winchHangMotor(winchHangMotorPort),
        //winchHangMotor2(winchHangMotor2Port),
        suckerMotor = new CANJaguar(suckerMotorPort /*,CANJaguar::kCurrent*/);
        suckerMotor2 = new CANJaguar(suckerMotorPort2 /*,CANJaguar::kCurrent*/);
        compressor = new Compressor(pressureSwitchChannel, compressorRelayChannel);
        kickLimit = new DigitalInput(kickLimitPort);
        kickerSolenoid = new Solenoid(kickerSolenoidChannel);
        kickerForwardSolenoid = new Solenoid(kickerForwardSolenoidChannel);
        /*frEncoder = new Encoder(frEncoderPort1, frEncoderPort2, true);
        flEncoder = new Encoder(flEncoderPort1, flEncoderPort2, true);
        brEncoder = new Encoder(brEncoderPort1, brEncoderPort2);
        blEncoder = new Encoder(blEncoderPort1, blEncoderPort2, true);*/
        dsLCD = DriverStationLCD.getInstance();
        //armServo(armServoPort);
        compressor.start();
        brMotorPower = 0;
        blMotorPower = 0;
        frMotorPower = 0;
        suckerPower = 0;
        winchShootPower = 0.0;
        kickerValue = false;
        armValue = hangerBreakValue = false;
        //winchBrakeValue = Relay::kOff;
        scriptMode = 0;
        /*frEncoder.start();
        flEncoder.start();
        brEncoder.start();
        blEncoder.start();*/
        shootWinchBreaker = false;
    }

    public boolean getWinchBreaker() { return shootWinchBreaker; }
    public boolean getKickLimit() { return kickLimitValue; }
    public double getSuckerCurrent() { return (suckerMotor.getOutputCurrent() + suckerMotor2.getOutputCurrent()) / 2.0; }


    static double clamp(double x)
    {
	if (x > 1.0) return 1.0;
	if (x < -1.0) return -1.0;
	return x;
    }

    public void stopMotors()
    {
	setSuckerPower(0);
	setWinchShootPower(0);
	//setWinchHangPower(0);
	setFLPower(0);
	setBLPower(0);
	setFRPower(0);
	setBRPower(0);
    }

    public void initAutonomous(PyroMain main)
    {

    }
    public void initOperator(PyroMain main)
    {

    }
    public void runAutonomous(PyroMain main)
    {
        updateValues(main);
    }
    public void runOperator(PyroMain main)
    {
        updateValues(main);
    }
    
    public void runDiagnostics(PyroMain main)
    {
        updateValues(main);
    }

    private void updateValues(PyroMain main)
    {
        dsLCD.println(DriverStationLCD.Line.kUser2, 1, "DIAG=".concat(String.valueOf(main.isDiagnosticsModeEnabled())));
        //dsLCD.println(DriverStationLCD.Line.kUser3, 1, "AUTO=".concat(String.valueOf(main.control.getAutoMode())));
        dsLCD.println(DriverStationLCD.Line.kUser3, 1, "TRIGGER=".concat(String.valueOf(hStick.getTrigger())));
        dsLCD.updateLCD();

        kickLimitValue = kickLimit.get();
        frMotor.set(-clamp(frMotorPower));
        flMotor.set(clamp(flMotorPower));
        brMotor.set(-clamp(brMotorPower));
        blMotor.set(clamp(blMotorPower));

        /*tmr = 0;
        if (tmr++ > 10)
        {
                tmr = 0;
                
        }*/

        suckerMotor.set(suckerPower);
        suckerMotor2.set(suckerPower);

        //if (getShooterCurrent() > shootWinchBreakerCurrent) { shootWinchBreaker = true; System.out.println("breaker tripped\n"); }
        if (winchShootPower > 0)
                shootWinchBreaker = false;

        //shootWinchBreaker = false; // debug

        if (!shootWinchBreaker) {
                winchShootMotor.set(winchShootPower);
        } else { // "breaker" tripped
                if (winchShootPower > 0) // positive is OK when tripped
                        winchShootMotor.set(winchShootPower);
                else
                        winchShootMotor.set(0.0);
        }        
        kickerSolenoid.set(kickerValue);
        kickerForwardSolenoid.set(kickerForwardValue);
    }

    public void setFRPower(double pwr) { frMotorPower = pwr; }
    public void setFLPower(double pwr) { flMotorPower = pwr; }
    public void setBRPower(double pwr) { brMotorPower = pwr; }
    public void setBLPower(double pwr) { blMotorPower = pwr; }

    public void setSuckerPower(double pwr)
    {
        suckerPower = -pwr;
    }

    public void setWinchShootPower(double pwr) { winchShootPower = pwr; }
    public void setKickerLatch(boolean value) { kickerValue = value; }
    public boolean getKickerLatch() { return kickerValue; }

    public void setKickerForwardValue(boolean value) { kickerForwardValue = value; }


    public double getFRPower() { return -frMotor.get(); }
    public double getFLPower() { return flMotor.get(); }
    public double getBRPower() { return -brMotor.get(); }
    public double getBLPower() { return blMotor.get(); }
    public double getSuckerPower() {return suckerMotor.get(); }
    public double getWinchShootPower() { return /*winchShootMotor.get();*/ 0; }

    public double getNewFRPower() { return frMotorPower; }
    public double getNewFLPower() { return flMotorPower; }
    public double getNewBRPower() { return brMotorPower; }
    public double getNewBLPower() { return blMotorPower; }
    public double getNewSuckerPower() {return suckerPower; }
    public double getNewWinchShootPower() {return winchShootPower; }
    public boolean getNewKickerValue() {return kickerValue; }
    //Relay::Value getNewWinchBrakeValue() {return winchBrakeValue;}

    // joystick Y axis thought to require negation
    public double getForward() {return -lStick.getY();}
    public double getStrafe() {return lStick.getX();}
    public double getTurn() {return rStick.getX();}
    public boolean getLeftTrigger() {return lStick.getTrigger();}
    public boolean getRightTrigger() {return rStick.getTrigger();}

    /*public int getFREncoder() { return frEncoder.get(); }
    public int getFLEncoder() { return flEncoder.get(); }
    public int getBREncoder() { return brEncoder.get(); }
    public int getBLEncoder() { return blEncoder.get(); }*/

    //temporary
    public boolean getOverBumpButton() {return false; }
    public boolean getWinchHangButton() {return hStick.getRawButton(4); }
    public boolean getKickerButton() {return hStick.getRawButton(2); }
    public boolean getScriptAbortButton() {return hStick.getRawButton(3); }

    public double getShooterCurrent() {
        return winchShootMotor.getOutputCurrent();
    }

    //added by rohan for diagnostic purposes
    /*public double getWinchFPCurrent() { return winchHangMotor.getOutputCurrent(); return 0; }*/
    /*public double getWinchCIMCurrent() { return winchHangMotor2.getOutputCurrent(); return 0; }*/
    //public double getArmServoAngle() {return armServo.getAngle();}

    //public double getGyroValue() { return 0; } // { return gyroValue; }
    //public void resetGyro() { } // { gyro.reset(); }
    /*public void resetEncoders()
    {
        frEncoder.reset();
        flEncoder.reset();
        brEncoder.reset();
        blEncoder.reset();
    }*/

}
