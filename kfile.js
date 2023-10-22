const project = new Project('RayTraceTest');
await project.addProject('Kinc');

project.addFile('Sources/**');
project.setDebugDir('Deployment');
project.addDefine('KORE_VKRT');

project.flatten();
resolve(project);
