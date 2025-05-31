const connection = require('../db');

function getInstalledAppsByDeviceId(deviceId, callback) {
    const query = `
        SELECT installed_apps.*, 
		   COALESCE(apps_info.icon_base64, NULL) AS app_icon 
	FROM installed_apps 
	LEFT JOIN apps_info ON installed_apps.exe_path = apps_info.exe_path
	WHERE installed_apps.device_id = ?`;
    connection.query(query, [deviceId], (err, results) => {
        if (err) {
            return callback(err, null);
        }
        callback(null, results);
    });
}

module.exports = {
    getInstalledAppsByDeviceId
};
