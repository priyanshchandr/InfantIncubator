// Function to fetch temperature data
async function fetchTemperature() {
    try {
        const response = await fetch('http://localhost:3000/temperature'); // Replace with your server's endpoint
        const temperatureValue = await response.text();
        document.getElementById("temperatureValue").textContent = temperatureValue + " °C";
    } catch (error) {
        console.error("Error fetching temperature data: " + error);
    }
}

// Function to fetch weight data
async function fetchWeight() {
    try {
        const response = await fetch('http://localhost:3000/weight'); // Replace with your server's endpoint
        const weightValue = await response.text();
        document.getElementById("weightValue").textContent = weightValue + " gm";
    } catch (error) {
        console.error("Error fetching weight data: " + error);
    }
}

// Function to fetch humidity data
async function fetchHumidity() {
    try {
        const response = await fetch('http://localhost:3000/humidity'); // Replace with your server's endpoint
        const humidityValue = await response.text();
        document.getElementById("humidityValue").textContent = humidityValue + "%";
    } catch (error) {
        console.error("Error fetching humidity data: " + error);
    }
}

// Function to fetch all sensor data
async function fetchSensorData() {
    fetchTemperature();
    fetchWeight();
    fetchHumidity();
}

// Fetch and update data on page load
fetchSensorData();

// Update data every 5 seconds
setInterval(fetchSensorData, 1000);
