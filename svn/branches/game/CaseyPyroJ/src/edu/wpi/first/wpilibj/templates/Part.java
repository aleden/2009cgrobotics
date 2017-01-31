/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.wpi.first.wpilibj.templates;

/**
 *
 * @author Currey-WilsonC
 */
public interface Part {
    public void initAutonomous(PyroMain main);
    public void initOperator(PyroMain main);
    public void runAutonomous(PyroMain main);
    public void runOperator(PyroMain main);
    public void runDiagnostics(PyroMain main);
}
