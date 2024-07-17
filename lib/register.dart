import 'package:firebase_auth/firebase_auth.dart';
import 'package:flutter/material.dart';
import 'package:flutter_application_1/auth.dart';
import 'package:cloud_firestore/cloud_firestore.dart';

class Register extends StatefulWidget {
  const Register({Key? key}) : super(key: key);

  @override
  _RegisterState createState() => _RegisterState();
}

class _RegisterState extends State<Register> {
  final FirebaseAuthServices _auth = FirebaseAuthServices();

  TextEditingController _usernameController = TextEditingController();
  TextEditingController _emailController = TextEditingController();
  TextEditingController _passwordController = TextEditingController();

  @override
  void dispose() {
    _usernameController.dispose();
    _emailController.dispose();
    _passwordController.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return Container(
      decoration: const BoxDecoration(
        image: DecorationImage(
            image: AssetImage('assets/(bg)Register.png'), fit: BoxFit.cover),
      ),
      child: Scaffold(
        backgroundColor: Colors.transparent,
        body: Stack(
          children: [
            Container(
              alignment: const Alignment(-0.7, -0.4),
              child: const Text(
                'Create\nAccount',
                style: TextStyle(
                  fontFamily: "Railway",
                  fontWeight: FontWeight.bold,
                  color: Colors.black,
                  fontSize: 33,
                ),
              ),
            ),
            SingleChildScrollView(
              child: Container(
                padding: EdgeInsets.only(
                    top: MediaQuery.of(context).size.height * 0.5,
                    right: 35,
                    left: 35),
                child: Column(
                  children: [
                    TextField(
                      controller: _usernameController,
                      decoration: InputDecoration(
                        fillColor: Colors.grey.shade300,
                        filled: true,
                        hintText: 'Username',
                        border: OutlineInputBorder(
                            borderRadius: BorderRadius.circular(10),
                            borderSide: BorderSide.none),
                      ),
                    ),
                    const SizedBox(
                      height: 10,
                    ),
                    TextField(
                      controller: _emailController,
                      decoration: InputDecoration(
                        fillColor: Colors.grey.shade300,
                        filled: true,
                        hintText: 'Email',
                        border: OutlineInputBorder(
                            borderRadius: BorderRadius.circular(10),
                            borderSide: BorderSide.none),
                      ),
                    ),
                    const SizedBox(
                      height: 10,
                    ),
                    TextField(
                      controller: _passwordController,
                      obscureText: true,
                      decoration: InputDecoration(
                        fillColor: Colors.grey.shade300,
                        filled: true,
                        hintText: 'Password',
                        border: OutlineInputBorder(
                            borderRadius: BorderRadius.circular(10),
                            borderSide: BorderSide.none),
                      ),
                    ),
                    const SizedBox(
                      height: 5,
                    ),
                    const SizedBox(
                      height: 10,
                    ),
                    Row(
                        mainAxisAlignment: MainAxisAlignment.spaceBetween,
                        children: [
                          const Text(
                            'Sign Up',
                            style: TextStyle(
                                fontFamily: "Railway",
                                color: Color(0xff4c505b),
                                fontSize: 27,
                                fontWeight: FontWeight.w700),
                          ),
                          CircleAvatar(
                            radius: 30,
                            backgroundColor: const Color(0xff4c505b),
                            child: IconButton(
                              color: Colors.white,
                              onPressed: _signUp,
                              icon: const Icon(Icons.arrow_forward),
                            ),
                          ),
                        ]),
                    const SizedBox(
                      height: 40,
                    ),
                    Row(
                      mainAxisAlignment: MainAxisAlignment.spaceBetween,
                      children: [
                        const Text(
                          'Already have account?',
                          style: TextStyle(
                              fontFamily: "Railway",
                              fontSize: 15,
                              color: Colors.black),
                        ),
                        TextButton(
                          onPressed: () {
                            Navigator.pushNamed(context, '/');
                          },
                          child: const Text(
                            'Sign In',
                            style: TextStyle(
                                fontFamily: "Railway",
                                decoration: TextDecoration.underline,
                                fontSize: 15,
                                color: Color(0xff4c505b)),
                          ),
                        ),
                      ],
                    )
                  ],
                ),
              ),
            ),
          ],
        ),
      ),
    );
  }

  void _signUp() async {
  String username = _usernameController.text;
  String email = _emailController.text;
  String password = _passwordController.text;

  try {
    UserCredential userCredential = await FirebaseAuth.instance.createUserWithEmailAndPassword(
      email: email,
      password: password,
    );

    User? user = userCredential.user;

    if (user!= null) {
      print("User created successfully: $user");
      FirebaseFirestore db = FirebaseFirestore.instance;
      String uid = user.uid;

      // Create a new document for the user
      await db.collection('users').doc(uid).set({
        'username': username,
        'email': email,
        // Add more fields as needed
      });

      print("User document created in Firestore");

      try {
        await _cloneCollection(uid);
      } catch (e) {
        print("Error cloning collection: $e");
      }

      print("User created and Firestore collection initialized for user: $uid");
      Navigator.pushNamed(context, "/bottomnav");
    } else {
      print("User registration failed");
    }
  } catch (e) {
    print("Error creating user: $e");
  }
}

Future<void> _cloneCollection(String uid) async {
  print("Cloning collection for user: $uid");
  try {
    // Authenticate the user
    User? user = FirebaseAuth.instance.currentUser;
    if (user == null) {
      // Sign in the user anonymously
      user = (await FirebaseAuth.instance.signInAnonymously()).user;
    }

    String? token;
    if (user != null) {
      token = await user.getIdToken();
    } else {
      // Handle the case where user is null
    }

    // Use the authentication token to authenticate the get() call
    FirebaseFirestore db = FirebaseFirestore.instance;
    db.useFirestoreEmulator('localhost', 8080);
    db.settings = const Settings(persistenceEnabled: true, sslEnabled: true);

    CollectionReference sourceCollection = db.collection('EsPData');
    CollectionReference targetCollection = db.collection('users').doc(uid).collection('EsPData');

    // Add a delay to ensure the document has been written to Firestore
    await Future.delayed(Duration(seconds: 1));

    QuerySnapshot snapshot = await sourceCollection.get(GetOptions(source: Source.server));

    print("Number of documents in source collection: ${snapshot.docs.length}");

    if (snapshot.docs.isEmpty) {
      print("No documents found in source collection");
    } else {
      for (var doc in snapshot.docs) {
        await targetCollection.doc(doc.id).set(doc.data());
      }

      print("Documents cloned from 'EsPData' to 'EsPData' for user: $uid");
    }
  } catch (e) {
    print("Error cloning collection: $e");
  }
}
}