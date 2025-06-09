const connection = require('../db');

function getInstalledAppsByDeviceId(deviceId, callback) {
    const query = `
        SELECT installed_apps.*, 
		   COALESCE(apps_info.icon_base64, NULL) AS app_icon 
	FROM installed_apps 
	LEFT JOIN apps_info ON installed_apps.install_location = apps_info.install_location
	WHERE installed_apps.device_id = ?`;
    connection.query(query, [deviceId], (err, results) => {
        if (err) {
            return callback(err, null);
        }
        callback(null, results);
    });
}

function addInstalledApps(deviceId, app_name, exe_path, install_location, publisher, version, uninstall_string, quiet_uninstall_string, status, callback) {
    // Kiểm tra xem app đã tồn tại chưa
    const checkQuery = `
        SELECT id FROM installed_apps 
        WHERE device_id = ? AND app_name = ?`;
    
    connection.query(checkQuery, [deviceId, app_name], (err, results) => {
        if (err) {
            return callback(err);
        }

        if (results.length > 0) {
            // Nếu đã tồn tại, chỉ update status
            const updateQuery = `
                UPDATE installed_apps 
                SET status = ?
                WHERE device_id = ? AND app_name = ?`;
            
            connection.query(updateQuery, [status, deviceId, app_name], (err, result) => {
                if (err) {
                    return callback(err);
                }
                callback(null, { updated: true, result });
            });
        } else {
            // Nếu chưa tồn tại, insert mới với đầy đủ các tham số
            const insertQuery = `
                INSERT INTO installed_apps (
                    device_id, 
                    app_name, 
                    exe_path, 
                    install_location, 
                    publisher, 
                    version, 
                    uninstall_string, 
                    quiet_uninstall_string, 
                    status
                ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)`;
            
            connection.query(insertQuery, [
                deviceId, 
                app_name, 
                exe_path, 
                install_location, 
                publisher, 
                version, 
                uninstall_string, 
                quiet_uninstall_string, 
                status
            ], (err, result) => {
                if (err) {
                    return callback(err);
                }
                callback(null, { inserted: true, result });
            });
        }
    });
}

function deleteAppByDeviceId(deviceId, callback) {
    const query = `
        DELETE FROM installed_apps 
        WHERE device_id = ?`;
    connection.query(query, [deviceId], (err, result) => {
        if (err) {
            return callback(err);
        }
        callback(null, result);
    });
}

module.exports = {
    getInstalledAppsByDeviceId,
    addInstalledApps,
    deleteAppByDeviceId
};
