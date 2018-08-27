package project.main.classes;

import java.awt.event.KeyEvent;

public class KeyListener implements java.awt.event.KeyListener{

	@Override
	public void keyPressed(KeyEvent e) {
		System.out.println(e.getKeyCode());
	}

	@Override
	public void keyReleased(KeyEvent e) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void keyTyped(KeyEvent e) {
		// TODO Auto-generated method stub
		
	}

}
