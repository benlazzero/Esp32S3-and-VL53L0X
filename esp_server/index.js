const express = require('express');
const fs = require('fs');
const path = require('path');
const app = express();

app.post('/upload', (req, res) => {
  const chunks = [];

  req.on('data', (chunk) => {
    console.log("Received chunk #: %d", chunks.length);
    chunks.push(chunk);
  });

  req.on('error', (err) => {
    console.error('Error received:', err);
  });

  req.on('end', () => {
    const imageBuffer = Buffer.concat(chunks);
    console.log("Total image size is %d bytes", imageBuffer.length);
        
    const filename = `upload_${Date.now()}.jpg`;
    const filePath = path.join(__dirname, 'uploads', filename);
    
    fs.writeFile(filePath, imageBuffer, (err) => {
        if (err) {
            console.error(err);
            res.status(500).send('Server Error');
        } else {
            console.log("success");
            res.status(200).send('Image uploaded successfully');
        }
     });
   });
});

app.listen(3500, () => console.log('Server running on port 3500'));

