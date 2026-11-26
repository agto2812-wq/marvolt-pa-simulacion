function cfg = marvolt_load_config()
  % Loads shared MARVOLT-PA parameter files from the repository config folder.
  % Run this project with GNU Octave 6.2+ (jsondecode support).

  this_dir = fileparts(mfilename('fullpath'));
  project_root = fileparts(fileparts(this_dir));
  config_dir = fullfile(project_root, 'config');

  physical_file = fullfile(config_dir, 'physical_parameters.json');
  mechanical_file = fullfile(config_dir, 'mechanical_parameters.json');
  experiment_file = fullfile(config_dir, 'experiment_defaults.json');

  required_files = {physical_file, mechanical_file, experiment_file};
  for idx = 1:numel(required_files)
    if exist(required_files{idx}, 'file') ~= 2
      error('No se encontró el archivo de configuración: %s', required_files{idx});
    endif
  endfor

  cfg.physical = jsondecode(fileread(physical_file));
  cfg.mechanical = jsondecode(fileread(mechanical_file));
  cfg.experiment = jsondecode(fileread(experiment_file));
endfunction
