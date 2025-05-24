link = document.createElement('a');
file = new Blob([JSON.stringify(cvars)], {type: 'text/plain'});
link.href = URL.createObjectURL(file);
link.download = 'data.json';
link.click();
URL.revokeObjectURL(link.href)
