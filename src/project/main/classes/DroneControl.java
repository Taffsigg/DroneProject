package project.main.classes;

import java.awt.EventQueue;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.Scanner;

import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JFrame;
import javax.swing.JOptionPane;
import javax.swing.JPanel;

import org.lwjgl.input.Controller;
import org.lwjgl.input.Controllers;

import com.fazecast.jSerialComm.SerialPort;
import javax.swing.JTextField;
import javax.swing.JLabel;
import javax.swing.JSlider;
import javax.swing.JToggleButton;
import javax.swing.JTextPane;

public class DroneControl {
	String s;
	private JComboBox<String> comboBox;
	private JFrame frame;
	private JPanel panel;
	static SerialPort chosenPort;
	private boolean sending;
	Thread thread2 = null;
	String data;
	private JButton btnNewButton;
	static Controller controller;
	private JTextField textField;
	JToggleButton tglbtnNewToggleButton;
	JSlider slider = new JSlider();
	JSlider slider_1 = new JSlider();
	JSlider slider_2 = new JSlider();
	JSlider slider_3 = new JSlider();
	private JTextField textField_1;
    JTextPane textPane;
    JButton btnNewButton_1;
    KeyListener kl;
    
	/**
	 * Launch the application.
	 */
	public static void main(String[] args) {
		EventQueue.invokeLater(new Runnable() {
			public void run() {
				try {
					DroneControl window = new DroneControl();
					window.frame.setVisible(true);
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		});
	}

	/**
	 * Create the application.
	 */
	public DroneControl() {
		initialize();
		
	}

	/**
	 * Initialize the contents of the frame.
	 */
	private void initialize() {
		frame = new JFrame();
		panel = new JPanel();
		frame.setBounds(100, 100, 1280, 720);
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frame.setTitle("Drone Alpha v0.1");
		frame.setResizable(false);
		panel.setLayout(null);
		frame.setContentPane(panel);
		comboBox = new JComboBox<String>();
		comboBox.setBounds(10, 11, 297, 20);
		panel.add(comboBox);
		kl = new KeyListener();
		


		try {
			Controllers.create();
			
			Controllers.poll();

			Controller cont = null;

			for (int i = 0; i < Controllers.getControllerCount(); i++) {
				controller = Controllers.getController(i);
				System.out.println(controller.getName() + " : " + controller.getIndex());
				if (controller.getName().equals("Mad Catz V.1 Stick")) {
					cont = Controllers.getController(i);
					for (int i1 = 0; i1 < cont.getButtonCount(); i1++) {
						System.out.println(cont.getButtonName(i1) + ": " + cont.isButtonPressed(i1));

					}
				}
			}

			controller = cont;

			
		} catch (Exception e) {
			e.printStackTrace();
		}

		SerialPort[] portNames = SerialPort.getCommPorts();
		for (int i = 0; i < portNames.length; i++)
			comboBox.addItem(portNames[i].getSystemPortName());

		JButton btnConnect = new JButton("Connect");
		btnConnect.addKeyListener(kl);
		btnConnect.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				if (btnConnect.getText().equals("Connect")) {
					if (!(comboBox.getItemCount() == 0)) {
						// attempt to connect to the serial port
						chosenPort = SerialPort.getCommPort(comboBox.getSelectedItem().toString());
						chosenPort.setComPortTimeouts(SerialPort.TIMEOUT_SCANNER, 0, 0);
						if (chosenPort.openPort()) {
							btnConnect.setText("Disconnect");
							comboBox.setEnabled(false);
						}

						// create a new thread that listens for incoming text
						// and populates the graph

						reciveInputfromSerial(true);
						sendInputToSerial(true);


					} else {
						JOptionPane.showMessageDialog(null, "No ports aviable!");
					}
				} else {
					// disconnect from the serial port
					chosenPort.closePort();
					comboBox.setEnabled(true);
					btnConnect.setText("Connect");
					reciveInputfromSerial(false);
					sendInputToSerial(false);

				}
			}
		});
		btnConnect.setBounds(10, 42, 89, 23);
		panel.add(btnConnect);

		btnNewButton = new JButton("LED ON");
		btnNewButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				if (btnNewButton.getText().equals("LED ON")) {
					data = "on";
					btnNewButton.setText("LED OFF");
				} else {
					data = "off";
					btnNewButton.setText("LED ON");
				}
			}
		});
		btnNewButton.setBounds(10, 80, 297, 23);
		panel.add(btnNewButton);

		textField = new JTextField();
		textField.setBounds(10, 114, 297, 20);
		panel.add(textField);
		textField.setColumns(10);

		JLabel lblController = new JLabel("Controller :");
		lblController.setBounds(10, 162, 123, 14);
		panel.add(lblController);
		slider.setMinimum(1000);

		slider.setValue(1500);
		slider.setMaximum(2000);
		slider.setBounds(86, 214, 200, 26);
		panel.add(slider);
		slider_1.setMinimum(1000);

		slider_1.setValue(1500);
		slider_1.setMaximum(2000);
		slider_1.setBounds(86, 262, 200, 26);
		panel.add(slider_1);
		slider_2.setMinimum(1000);

		slider_2.setValue(1500);
		slider_2.setMaximum(2000);
		slider_2.setBounds(86, 315, 200, 26);
		panel.add(slider_2);
		slider_3.setMinimum(1000);

		slider_3.setValue(1500);
		slider_3.setMaximum(2000);
		slider_3.setBounds(86, 366, 200, 26);
		panel.add(slider_3);

		JLabel lblThrottle = new JLabel("Throttle: ");
		lblThrottle.setBounds(10, 214, 66, 14);
		panel.add(lblThrottle);

		JLabel lblYaw = new JLabel("Yaw: ");
		lblYaw.setBounds(10, 262, 66, 14);
		panel.add(lblYaw);

		JLabel lblRoll = new JLabel("Roll: ");
		lblRoll.setBounds(10, 315, 66, 14);
		panel.add(lblRoll);

		JLabel lblPitch = new JLabel("Pitch: ");
		lblPitch.setBounds(10, 366, 66, 14);
		panel.add(lblPitch);

		tglbtnNewToggleButton = new JToggleButton("Start");
		tglbtnNewToggleButton.setBounds(86, 416, 200, 23);
		panel.add(tglbtnNewToggleButton);
		tglbtnNewToggleButton.setEnabled(false);
		
		textField_1 = new JTextField();
		textField_1.setBounds(388, 114, 271, 20);
		panel.add(textField_1);
		textField_1.setColumns(10);
		
		JLabel lblSend = new JLabel("Send");
		lblSend.setBounds(332, 117, 46, 14);
		panel.add(lblSend);
		
		JLabel lblRecive = new JLabel("Recive");
		lblRecive.setBounds(332, 162, 46, 14);
		panel.add(lblRecive);
		
		textPane = new JTextPane();
		textPane.setBounds(388, 162, 271, 301);
		panel.add(textPane);
		
		btnNewButton_1 = new JButton("Controller");
		btnNewButton_1.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				if(btnNewButton_1.getText().equals("Controller")){
					btnNewButton_1.setText("Keyboard");
				}else{
					btnNewButton_1.setText("Controller");
				}
			}
		});
		btnNewButton_1.setBounds(86, 450, 200, 23);
		panel.add(btnNewButton_1);
		btnNewButton_1.addKeyListener(kl);

	}

	public void sendInputToSerial(boolean start) {
		Thread thread = new Thread() {
			@Override
			public void run() {
				// wait after connecting, so the bootloader can finish
				try {
					Thread.sleep(100);
				} catch (Exception e) {
				}

				// enter an infinite loop that sends text to the arduino
				PrintWriter output = new PrintWriter(chosenPort.getOutputStream());

				data = null;

				sending = true;

				boolean startdrone = false;
				String sd = "0";
				
				int a = 1500,b = 1500,c = 1000,d = 1500;
				
				int se=15;
				int de=80;

				while (sending) {
					
					
					// System.out.println("data is: " + data);
        try{
        	
        	if(btnNewButton_1.getText().equals("Controller")) {
					if (controller.isButtonPressed(6)) {
						System.out.println("pressed");
						if (!startdrone) {
							startdrone = true;
							sd = "1";
							tglbtnNewToggleButton.setSelected(true);
//							data ="on";
						} else {
							startdrone = false;
							sd = "0";
							tglbtnNewToggleButton.setSelected(false);
//							data = "off";
						}

					}
					
					a = Math.round((controller.getXAxisValue() + 1) * 500 + 1000); 
					b = Math.round((controller.getYAxisValue() + 1) * 500+ 1000); 
					c = Math.round((-((controller.getZAxisValue() + 1) - 2)) * 500+ 1000); 
					d = Math.round((controller.getRZAxisValue() + 1) * 500+ 1000); 
					
				//System.out.println(controller.getYAxisValue());

					slider_3.setValue(b );
	                slider_2.setValue(a);
	                slider_1.setValue(d);
	                slider.setValue(c);
	                
        	}else{
        		
        		
                	 if(kl.w){
             			if(c < 2000){
             			    c+=kl.se;
             			}else{
             				c = 2000;
             			}
             			System.out.println(c);
             		}else{
             			if(!kl.s) {
             			if(c > 1000){
             			    c-=kl.de;
             			}else{
             				c = 1000;
             			}
             			}
             			System.out.println(c);
             		}
             		 if(kl.s){
                      	if(c > 1000){
              			    c-=de;
              			}else{
              				c = 1000;
              			}
              		}else{
              			if(!kl.w) {
              			if(c < 1500){
              			    c+=de;
              			}else{
              				c = 1500;
              			}
              			}
              		}
                 
        		
                if(kl.a){
                	if(d > 1000){
        			    d-=se;
        			}else{
        				d = 1000;
        			}
        		}else{
        			if(!kl.d) {		
        				d = 1500;
        			}
        		}
                if(kl.d){
                	if(d < 2000){
        			    d+=se;
        			}else{
        				d = 2000;
        			}
        		}else{
        			if(!kl.a) {
        		
        				d = 1500;
        			
        			}
        		}
                if(kl.up){
                	if(b > 1000){
        			    b-=se;
        			}else{
        				b = 1000;
        			}
        		}else{
        			if(!kl.down) {
        				b = 1500;
        			}
        			
        		}
                if(kl.down){
                	if(b < 2000){
        			    b+=se;
        			}else{
        				b = 2000;
        			}
        		}else{
        				if(!kl.up){
        					b = 1500;
            			}
        		}
                if(kl.left){
                	if(a > 1000){
        			    a-=se;
        			}else{
        				a = 1000;
        			}
        		}else{
        			if(!kl.right){
        		        a=1500;
        			}
        		}
                if(kl.right){
                	if(a < 2000){
        			    a+=se;
        			}else{
        				a = 2000;
        			}
        		}else{
        			if(!kl.left){
        		        a=1500;
        			}
        		}
                slider_3.setValue(b );
                slider_2.setValue(a);
                slider_1.setValue(d);
                slider.setValue(c);
        	}
					
        }catch(Exception e){
        	
        }
                   


					data = a + " " + b + " " + c + " " + d ;
					
					textField.setText("X : " + a + "   " + "Y : " + b + "   " + "Z : " + c + "  RZ : " + d
							+ " startdrone : " + sd);
					textField_1.setText(data);
					
                   if(btnNewButton_1.getText().equals("Controller")) {
					controller.poll();
                   }
					if (data != null) {
//						System.out.println("data is not null!");
						output.print(data);
						output.flush();
						try {
							Thread.sleep(100);
						} catch (Exception e) {
						}
//						System.out.println("Successfully Sending! " + data);
					}
				}
			}
		};
		if (start == true) {
			thread.start();
		} else {
			sending = false;
			thread.interrupt();
			System.out.println("send thread is alive:" + thread.isAlive());
		}
	}

	public void reciveInputfromSerial(boolean start) {
		thread2 = new Thread() {
			@Override
			public void run() {

				Scanner scanner = new Scanner(chosenPort.getInputStream());
				
				try {
					System.out.println(chosenPort.getInputStream().read());
					System.out.println("recive thread is alive:" + thread2.isAlive());
				} catch (IOException e1) {
					e1.printStackTrace();
				}
				while (scanner.hasNextLine()) {
					try {
						String line = scanner.nextLine();
					//	System.out.println("number is: " + line);
						textPane.setText(line);
					} catch (Exception e) {
					}
				}
				scanner.close();
			}
		};
		if (start == true) {
			thread2.start();
		} else {
			if (thread2.isAlive()) {
				thread2.interrupt();
				;
			}
		}
	}
}