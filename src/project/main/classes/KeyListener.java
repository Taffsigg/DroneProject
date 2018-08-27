package project.main.classes;

import java.awt.event.KeyEvent;

public class KeyListener implements java.awt.event.KeyListener{
	
	boolean w = false, up = false, s = false, down = false, a = false, left = false, d = false, right = false;
	int se = 20, de = 80;


	@Override
	public void keyPressed(KeyEvent e) {
		if(e.getKeyCode() == e.VK_D){
			d = true;
		}else if(e.getKeyCode() == e.VK_UP){
			up = true;
		}else if(e.getKeyCode() == e.VK_DOWN){
			down = true;
		}else if(e.getKeyCode() == e.VK_LEFT){
			left = true;
		}else if(e.getKeyCode() == e.VK_RIGHT){
			right = true;
		}else if(e.getKeyCode() == e.VK_W){
			w = true;
		}else if(e.getKeyCode() == e.VK_A){
			a = true;
		}else if(e.getKeyCode() == e.VK_S){
			s = true;
			System.out.println(s);
		}else if(e.isShiftDown()){
			se = 50;
			System.out.println(se);
		}
	}

	@Override
	public void keyReleased(KeyEvent e) {
		// TODO Auto-generated method stub
		if(e.getKeyCode() == e.VK_D){
			d = false;
		}else if(e.getKeyCode() == e.VK_UP){
			up = false;
		}else if(e.getKeyCode() == e.VK_DOWN){
			down = false;
		}else if(e.getKeyCode() == e.VK_LEFT){
			left = false;
		}else if(e.getKeyCode() == e.VK_RIGHT){
			right = false;
		}else if(e.getKeyCode() == e.VK_W){
			w = false;
		}else if(e.getKeyCode() == e.VK_A){
			a = false;
		}else if(e.getKeyCode() == e.VK_S){
			s = false;
			System.out.println(s);
		}else if(!e.isShiftDown()){
			se = 20;
			System.out.println(se);
		}
	}

	@Override
	public void keyTyped(KeyEvent e) {
		// TODO Auto-generated method stub
		
	}

}
