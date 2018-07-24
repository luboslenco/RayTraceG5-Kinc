let project = new Project('RayTraceTest');

project.addFile('Sources/**');
project.setDebugDir('Deployment');

resolve(project);
