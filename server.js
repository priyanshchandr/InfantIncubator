const express = require('express');
const fetch = require('node-fetch');

const app = express();
const port = 3000;

app.use(express.static('public'));

app.get('/temperature', async (req, res) => {
    try {
        const response = await fetch('http://192.168.101.35/temperature');
        const temperatureValue = await response.text();
        res.send(temperatureValue);
    } catch (error) {
        res.status(500).send('Error fetching temperature data');
    }
});

app.get('/humidity', async (req, res) => {
    try {
        const response = await fetch('http://192.168.101.35/humidity');
        const humidityValue = await response.text();
        res.send(humidityValue);
    } catch (error) {
        res.status(500).send('Error fetching humidity data');
    }
});

app.get('/weight', async (req, res) => {
    try {
        const response = await fetch('http://192.168.101.35/weight');
        const weightValue = await response.text();
        res.send(weightValue);
    } catch (error) {
        res.status(500).send('Error fetching weight data');
    }
});


app.listen(port, () => {
    console.log(`Proxy server running at http://localhost:${port}`);
});
