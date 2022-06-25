<?php
$servername = "localhost";
$dbname = "rover_data";
$username = "root";
$password = "";

// Keep this API Key value to be compatible with the ESP32 code provided in the project page. 
// If you change this value, the ESP32 sketch needs to match

$api_key_value = "tPmAT5Ab3j7F9";

$api_key = $rover_coord = $rover_angle = $fan_coord = $alien_coord = $alien_colour = $building_coord = "";

if ($_SERVER["REQUEST_METHOD"] == "POST") {
    $api_key = test_input($_POST["api_key"]);
    if($api_key == $api_key_value) {
        $rover_coord = test_input($_POST["rover_coord"]);
        $rover_angle = test_input($_POST["rover_angle"]);
        $fan_coord = test_input($_POST["fan_coord"]);
        $alien_coord = test_input($_POST["alien_coord"]);
        $alien_colour = test_input($_POST["alien_colour"]);
        $building_coord = test_input($_POST["building_coord"]);
        
        // Create connection
        $conn = new mysqli($servername, $username, $password, $dbname);
        // Check connection
        if ($conn->connect_error) {
            die("Connection failed: " . $conn->connect_error);
        } 
        
        $sql = "INSERT INTO post_data (rover_coord, rover_angle, fan_coord, alien_coord, alien_colour, building_coord)
        VALUES ('" . $rover_coord . "', '" . $rover_angle . "', '" . $fan_coord . "', '" . $alien_coord . "', '" . $alien_colour ."', '" . $building_coord . "')";
        
        if ($conn->query($sql) === TRUE) {
            echo "New record created successfully";
        } 
        else {
            echo "Error: " . $sql . "<br>" . $conn->error;
        }
    
        $conn->close();
    }
    else {
        echo "Wrong API Key provided.";
    }

}
else {
    echo "No data posted with HTTP POST.";
}

function test_input($data) {
    $data = trim($data);
    $data = stripslashes($data);
    $data = htmlspecialchars($data);
    return $data;
}