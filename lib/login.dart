import 'package:flutter/material.dart';

class Login extends StatefulWidget{
  const Login({Key? key}) : super (key: key);
  
  @override
  _LoginState createState() => _LoginState();
}


class _LoginState extends State<Login>{

  @override
  Widget build(BuildContext context){
    return Container(
      decoration: const BoxDecoration(
        image: DecorationImage(
          alignment: Alignment(0.0, -1.3),
          image: AssetImage('assets/logo.png'),
        ),
      ),
      child: Scaffold(
        backgroundColor: Colors.transparent,
        body: Stack(
          children: [
            Container(
              alignment: const Alignment(-0.7, -0.2),
              child: const Text('Welcome!', 
              style: TextStyle(
                fontFamily: "Railway", 
                fontWeight: FontWeight.bold, 
                color: Colors.black, 
                fontSize: 33,
                ),
              ),
            ),
            const SizedBox(height: 10),
            Container(
              alignment: const Alignment(-0.6, -0.1),
              child: const Text(
                "Please Sign In to Continue",
                style: TextStyle(
                  fontFamily: "Railway", 
                  fontWeight: FontWeight.bold, 
                  color: Colors.black, 
                  fontSize: 15
                ),
              ),
            ),
            SingleChildScrollView(
              child: Container(
              padding: EdgeInsets.only(
                top: MediaQuery.of(context).size.height*0.5,
                right: 35,
                left: 35),
              child: Column(
                children: [
                  TextField(
                    decoration: InputDecoration(
                      fillColor: const Color.fromRGBO(160, 199, 235, 1),
                      filled: true,
                      hintText: 'Email',
                      border: OutlineInputBorder(
                        borderRadius: BorderRadius.circular(10)
                      ),
                    ),
                  ),
                  const SizedBox(
                    height: 10,
                  ),
                  TextField(
                    obscureText: true,
                    decoration: InputDecoration(
                      fillColor: const Color.fromRGBO(160, 199, 235, 1),
                      filled: true,
                      hintText: 'Password',
                      border: OutlineInputBorder(
                        borderRadius: BorderRadius.circular(10)
                      ),
                    ),
                  ),
                  const SizedBox(
                    height: 10,
                  ),
                  TextButton(onPressed: () {},
                      child: Text('Forgot Password?', style: TextStyle(
                        decoration: TextDecoration.underline,
                        fontSize: 18,
                        color: Color(0xff4c505b)
                      ),),),
                  const SizedBox(
                    height: 10,
                  ),
                  Row(
                    mainAxisAlignment: MainAxisAlignment.spaceBetween,
                    children: [
                      Text(
                        'Sign In',
                        style: TextStyle(
                          color: Color(0xff4c505b),
                          fontSize: 27, 
                          fontWeight: FontWeight.w700
                        ),
                      ),
                      CircleAvatar(
                        radius: 30,
                        backgroundColor: Color(0xff4c505b),
                        child: IconButton(
                          color: Colors.white,
                          onPressed: () {},
                          icon: Icon(Icons.arrow_forward),
                        ),
                      ),
                    ]
                  ),
                  const SizedBox(
                    height: 40,
                  ),
                  Row(
                    children: [
                      TextButton(onPressed: () {},
                      child: Text('Dont have account yet?', style: TextStyle(
                        fontSize: 18,
                        color: Colors.black
                      ),),),
                      TextButton(onPressed: () {},
                      child: Text('Sign Up', style: TextStyle(
                        decoration: TextDecoration.underline,
                        fontSize: 18,
                        color: Color(0xff4c505b)
                      ),),)
                    ]
                  )
                ]
              )
            )
            )
          ]
        ),
      ),
    );
  }
}