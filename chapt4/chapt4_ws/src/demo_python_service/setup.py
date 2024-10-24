from setuptools import find_packages, setup

package_name = 'demo_python_service'

setup(
    name=package_name,
    version='0.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
        ('share/'+package_name+"/resource",['resource/default.jpeg']),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='zkhl',
    maintainer_email='15230093114@163.com',
    description='TODO: Package description',
    license='Apache-2.0',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            'learn_face_detect_node=demo_python_service.learn_face_detect:main'
        ],
    },
)
