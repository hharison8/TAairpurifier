const functions = require("firebase-functions");
const admin = require("firebase-admin");
admin.initializeApp();

exports.createUserCollection = functions.auth.user().onCreate(async (user) => {
  const uid = user.uid;
  const db = admin.firestore();
  const mainCollectionRef = db.collection("EsPData"); // Main collection
  const userCollectionRef = db.collection(`users/${uid}/clonedCollection`); // New user collection

  try {
    const snapshot = await mainCollectionRef.get();
    const batch = db.batch();

    snapshot.forEach((doc) => {
      const newDocRef = userCollectionRef.doc(doc.id);
      batch.set(newDocRef, doc.data());
    });

    await batch.commit();
    console.log(`Cloned collection for user ${uid}`);
  } catch (error) {
    console.error("Error cloning collection: ", error);
  }
});
